/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qsimplex_p.h"

#include <QtCore/qset.h>
#include <QtCore/qdebug.h>

#include <stdlib.h>

QT_BEGIN_NAMESPACE

/*!
  \internal
  \class QSimplex

  The QSimplex class is a Linear Programming problem solver based on the two-phase
  simplex method.

  It takes a set of QSimplexConstraints as its restrictive constraints and an
  additional QSimplexConstraint as its objective function. Then methods to maximize
  and minimize the problem solution are provided.

  The two-phase simplex method is based on the following steps:
  First phase:
  1.a) Modify the original, complex, and possibly not feasible problem, into a new,
       easy to solve problem.
  1.b) Set as the objective of the new problem, a feasible solution for the original
       complex problem.
  1.c) Run simplex to optimize the modified problem and check whether a solution for
       the original problem exists.

  Second phase:
  2.a) Go back to the original problem with the feasibl (but not optimal) solution
       found in the first phase.
  2.b) Set the original objective.
  3.c) Run simplex to optimize the original problem towards its optimal solution.
*/

QSimplex::QSimplex() : objective(0), rows(0), columns(0), firstArtificial(0), matrix(0)
{
}

QSimplex::~QSimplex()
{
    clearDataStructures();
}

void QSimplex::clearDataStructures()
{
    if (matrix == 0)
        return;

    // Matrix
    rows = 0;
    columns = 0;
    firstArtificial = 0;
    free(matrix);
    matrix = 0;

    // Constraints
    for (int i = 0; i < constraints.size(); ++i) {
        delete constraints[i]->helper.first;
        constraints[i]->helper.first = 0;
        constraints[i]->helper.second = 0.0;
        delete constraints[i]->artificial;
        constraints[i]->artificial = 0;
    }
    constraints.clear();

    // Other
    variables.clear();
    objective = 0;
}

/*!
  \internal
  Sets the new constraints in the simplex solver and returns whether the problem
  is feasible.

  This method sets the new constraints, normalizes them, creates the simplex matrix
  and runs the first simplex phase.
*/
bool QSimplex::setConstraints(const QList<QSimplexConstraint *> newConstraints)
{
    ////////////////////////////
    // Reset to initial state //
    ////////////////////////////
    clearDataStructures();

    if (newConstraints.isEmpty())
        return true;    // we are ok with no constraints
    constraints = newConstraints;

    ///////////////////////////////////////
    // Prepare variables and constraints //
    ///////////////////////////////////////

    // Set Variables direct mapping.
    // "variables" is a list that provides a stable, indexed list of all variables
    // used in this problem.
    QSet<QSimplexVariable *> variablesSet;
    for (int i = 0; i < constraints.size(); ++i)
        variablesSet += \
            QSet<QSimplexVariable *>::fromList(constraints[i]->variables.keys());
    variables = variablesSet.toList();

    // Set Variables reverse mapping
    // We also need to be able to find the index for a given variable, to do that
    // we store in each variable its index.
    for (int i = 0; i < variables.size(); ++i) {
        // The variable "0" goes at the column "1", etc...
        variables[i]->index = i + 1;
    }

    // Normalize Constraints
    // In this step, we prepare the constraints in two ways:
    // Firstly, we modify all constraints of type "LessOrEqual" or "MoreOrEqual"
    // by the adding slack or surplus variables and making them "Equal" constraints.
    // Secondly, we need every single constraint to have a direct, easy feasible
    // solution. Constraints that have slack variables are already easy to solve,
    // to all the others we add artificial variables.
    //
    // At the end we modify the constraints as follows:
    //  - LessOrEqual: SLACK variable is added.
    //  - Equal: ARTIFICIAL variable is added.
    //  - More or Equal: ARTIFICIAL and SURPLUS variables are added.
    int variableIndex = variables.size();
    QList <QSimplexVariable *> artificialList;

    for (int i = 0; i < constraints.size(); ++i) {
        QSimplexVariable *slack;
        QSimplexVariable *surplus;
        QSimplexVariable *artificial;

        Q_ASSERT(constraints[i]->helper.first == 0);
        Q_ASSERT(constraints[i]->artificial == 0);

        switch(constraints[i]->ratio) {
        case QSimplexConstraint::LessOrEqual:
            slack = new QSimplexVariable;
            slack->index = ++variableIndex;
            constraints[i]->helper.first = slack;
            constraints[i]->helper.second = 1.0;
            break;
        case QSimplexConstraint::MoreOrEqual:
            surplus = new QSimplexVariable;
            surplus->index = ++variableIndex;
            constraints[i]->helper.first = surplus;
            constraints[i]->helper.second = -1.0;
            // fall through
        case QSimplexConstraint::Equal:
            artificial = new QSimplexVariable;
            constraints[i]->artificial = artificial;
            artificialList += constraints[i]->artificial;
            break;
        }
    }

    // All original, slack and surplus have already had its index set
    // at this point. We now set the index of the artificial variables
    // as to ensure they are at the end of the variable list and therefore
    // can be easily removed at the end of this method.
    firstArtificial = variableIndex + 1;
    for (int i = 0; i < artificialList.size(); ++i)
        artificialList[i]->index = ++variableIndex;
    artificialList.clear();

    /////////////////////////////
    // Fill the Simplex matrix //
    /////////////////////////////

    // One for each variable plus the Basic and BFS columns (first and last)
    columns = variableIndex + 2;
    // One for each constraint plus the objective function
    rows = constraints.size() + 1;

    matrix = (qreal *)malloc(sizeof(qreal) * columns * rows);
    if (!matrix) {
        qWarning() << "QSimplex: Unable to allocate memory!";
        return false;
    }
    for (int i = columns * rows - 1; i >= 0; --i)
        matrix[i] = 0.0;

    // Fill Matrix
    for (int i = 1; i <= constraints.size(); ++i) {
        QSimplexConstraint *c = constraints[i - 1];

        if (c->artificial) {
            // Will use artificial basic variable
            setValueAt(i, 0, c->artificial->index);
            setValueAt(i, c->artificial->index, 1.0);

            if (c->helper.second != 0.0) {
                // Surplus variable
                setValueAt(i, c->helper.first->index, c->helper.second);
            }
        } else {
            // Slack is used as the basic variable
            Q_ASSERT(c->helper.second == 1.0);
            setValueAt(i, 0, c->helper.first->index);
            setValueAt(i, c->helper.first->index, 1.0);
        }

        QHash<QSimplexVariable *, qreal>::const_iterator iter;
        for (iter = c->variables.constBegin();
             iter != c->variables.constEnd();
             ++iter) {
            setValueAt(i, iter.key()->index, iter.value());
        }

        setValueAt(i, columns - 1, c->constant);
    }

    // Set objective for the first-phase Simplex.
    // Z = -1 * sum_of_artificial_vars
    for (int j = firstArtificial; j < columns - 1; ++j)
        setValueAt(0, j, 1.0);

    // Maximize our objective (artificial vars go to zero)
    solveMaxHelper();

    // If there is a solution where the sum of all artificial
    // variables is zero, then all of them can be removed and yet
    // we will have a feasible (but not optimal) solution for the
    // original problem.
    // Otherwise, we clean up our structures and report there is
    // no feasible solution.
    if (valueAt(0, columns - 1) != 0.0) {
        qWarning() << "QSimplex: No feasible solution!";
        clearDataStructures();
        return false;
    }

    // Remove artificial variables. We already have a feasible
    // solution for the first problem, thus we don't need them
    // anymore.
    clearColumns(firstArtificial, columns - 2);

    #ifdef QT_DEBUG
    // Ensure that at the end of the simplex each row should either:
    //  - Have a positive value on the column associated to its variable, or
    //  - Have zero values in all columns.
    //
    // This avoids a regression where restrictions would be lost
    // due to randomness in the pivotRowForColumn method.
    for (int i = 1; i < rows; ++i) {
        int variableIndex = valueAt(i, 0);
        if (valueAt(i, variableIndex) > 0)
            continue;

        for (int j = 1; j < columns; ++j) {
            Q_ASSERT(valueAt(i, j) == 0);
        }
    }
    #endif

    return true;
}

/*!
  \internal

  Run simplex on the current matrix with the current objective.

  This is the iterative method. The matrix lines are combined
  as to modify the variable values towards the best solution possible.
  The method returns when the matrix is in the optimal state.
*/
void QSimplex::solveMaxHelper()
{
    reducedRowEchelon();
    while (iterate()) ;
}

void QSimplex::setObjective(QSimplexConstraint *newObjective)
{
    objective = newObjective;
}

void QSimplex::clearRow(int rowIndex)
{
    qreal *item = matrix + rowIndex * columns;
    for (int i = 0; i < columns; ++i)
        item[i] = 0.0;
}

void QSimplex::clearColumns(int first, int last)
{
    for (int i = 0; i < rows; ++i) {
        qreal *row = matrix + i * columns;
        for (int j = first; j <= last; ++j)
            row[j] = 0.0;
    }
}

void QSimplex::dumpMatrix()
{
    qDebug("---- Simplex Matrix ----\n");

    QString str(QLatin1String("       "));
    for (int j = 0; j < columns; ++j)
        str += QString::fromAscii("  <%1 >").arg(j, 2);
    qDebug("%s", qPrintable(str));
    for (int i = 0; i < rows; ++i) {
        str = QString::fromAscii("Row %1:").arg(i, 2);

        qreal *row = matrix + i * columns;
        for (int j = 0; j < columns; ++j)
            str += QString::fromAscii("%1").arg(row[j], 7, 'f', 2);
        qDebug("%s", qPrintable(str));
    }
    qDebug("------------------------\n");
}

void QSimplex::combineRows(int toIndex, int fromIndex, qreal factor)
{
    if (!factor)
        return;

    qreal *from = matrix + fromIndex * columns;
    qreal *to = matrix + toIndex * columns;

    for (int j = 1; j < columns; ++j) {
        qreal value = from[j];

        // skip to[j] = to[j] + factor*0.0
        if (value == 0.0)
            continue;

        to[j] += factor * value;

        // ### Avoid Numerical errors
        if (qAbs(to[j]) < 0.0000000001)
            to[j] = 0.0;
    }
}

int QSimplex::findPivotColumn()
{
    qreal min = 0;
    int minIndex = -1;

    for (int j = 0; j < columns-1; ++j) {
        if (valueAt(0, j) < min) {
            min = valueAt(0, j);
            minIndex = j;
        }
    }

    return minIndex;
}

/*!
  \internal

  For a given pivot column, find the pivot row. That is, the row with the
  minimum associated "quotient" where:

  - quotient is the division of the value in the last column by the value
    in the pivot column.
  - rows with value less or equal to zero are ignored
  - if two rows have the same quotient, lines are chosen based on the
    highest variable index (value in the first column)

  The last condition avoids a bug where artificial variables would be
  left behind for the second-phase simplex, and with 'good'
  constraints would be removed before it, what would lead to incorrect
  results.
*/
int QSimplex::pivotRowForColumn(int column)
{
    qreal min = qreal(999999999999.0); // ###
    int minIndex = -1;

    for (int i = 1; i < rows; ++i) {
        qreal divisor = valueAt(i, column);
        if (divisor <= 0)
            continue;

        qreal quotient = valueAt(i, columns - 1) / divisor;
        if (quotient < min) {
            min = quotient;
            minIndex = i;
        } else if ((quotient == min) && (valueAt(i, 0) > valueAt(minIndex, 0))) {
            minIndex = i;
        }
    }

    return minIndex;
}

void QSimplex::reducedRowEchelon()
{
    for (int i = 1; i < rows; ++i) {
        int factorInObjectiveRow = valueAt(i, 0);
        combineRows(0, i, -1 * valueAt(0, factorInObjectiveRow));
    }
}

/*!
  \internal

  Does one iteration towards a better solution for the problem.
  See 'solveMaxHelper'.
*/
bool QSimplex::iterate()
{
    // Find Pivot column
    int pivotColumn = findPivotColumn();
    if (pivotColumn == -1)
        return false;

    // Find Pivot row for column
    int pivotRow = pivotRowForColumn(pivotColumn);
    if (pivotRow == -1) {
        qWarning() << "QSimplex: Unbounded problem!";
        return false;
    }

    // Normalize Pivot Row
    qreal pivot = valueAt(pivotRow, pivotColumn);
    if (pivot != 1.0)
        combineRows(pivotRow, pivotRow, (1.0 - pivot) / pivot);

    // Update other rows
    for (int row=0; row < rows; ++row) {
        if (row == pivotRow)
            continue;

        combineRows(row, pivotRow, -1 * valueAt(row, pivotColumn));
    }

    // Update first column
    setValueAt(pivotRow, 0, pivotColumn);

    //    dumpMatrix();
    //    qDebug("------------ end of iteration --------------\n");
    return true;
}

/*!
  \internal

  Both solveMin and solveMax are interfaces to this method.

  The enum solverFactor admits 2 values: Minimum (-1) and Maximum (+1).

  This method sets the original objective and runs the second phase
  Simplex to obtain the optimal solution for the problem. As the internal
  simplex solver is only able to _maximize_ objectives, we handle the
  minimization case by inverting the original objective and then
  maximizing it.
*/
qreal QSimplex::solver(solverFactor factor)
{
    // Remove old objective
    clearRow(0);

    // Set new objective
    QHash<QSimplexVariable *, qreal>::const_iterator iter;
    for (iter = objective->variables.constBegin();
         iter != objective->variables.constEnd();
         ++iter) {
        setValueAt(0, iter.key()->index, -1 * factor * iter.value());
    }

    solveMaxHelper();
    collectResults();

    return factor * valueAt(0, columns - 1);
}

/*!
  \internal
  Minimize the original objective.
*/
qreal QSimplex::solveMin()
{
    return solver(Minimum);
}

/*!
  \internal
  Maximize the original objective.
*/
qreal QSimplex::solveMax()
{
    return solver(Maximum);
}

/*!
  \internal

  Reads results from the simplified matrix and saves them in the
  "result" member of each QSimplexVariable.
*/
void QSimplex::collectResults()
{
    // All variables are zero unless overridden below.

    // ### Is this really needed? Is there any chance that an
    // important variable remains as non-basic at the end of simplex?
    for (int i = 0; i < variables.size(); ++i)
        variables[i]->result = 0;

    // Basic variables
    // Update the variable indicated in the first column with the value
    // in the last column.
    for (int i = 1; i < rows; ++i) {
        int index = valueAt(i, 0) - 1;
        if (index < variables.size())
            variables[index]->result = valueAt(i, columns - 1);
    }
}

QT_END_NAMESPACE