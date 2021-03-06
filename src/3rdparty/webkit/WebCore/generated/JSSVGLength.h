/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef JSSVGLength_h
#define JSSVGLength_h

#if ENABLE(SVG)

#include "JSDOMBinding.h"
#include "JSSVGPODTypeWrapper.h"
#include "SVGElement.h"
#include "SVGLength.h"
#include <runtime/JSGlobalObject.h>
#include <runtime/ObjectPrototype.h>

namespace WebCore {

class JSSVGLength : public DOMObjectWithGlobalPointer {
    typedef DOMObjectWithGlobalPointer Base;
public:
    JSSVGLength(NonNullPassRefPtr<JSC::Structure>, JSDOMGlobalObject*, PassRefPtr<JSSVGPODTypeWrapper<SVGLength> >);
    virtual ~JSSVGLength();
    static JSC::JSObject* createPrototype(JSC::ExecState*, JSC::JSGlobalObject*);
    virtual bool getOwnPropertySlot(JSC::ExecState*, const JSC::Identifier& propertyName, JSC::PropertySlot&);
    virtual bool getOwnPropertyDescriptor(JSC::ExecState*, const JSC::Identifier& propertyName, JSC::PropertyDescriptor&);
    virtual void put(JSC::ExecState*, const JSC::Identifier& propertyName, JSC::JSValue, JSC::PutPropertySlot&);
    virtual const JSC::ClassInfo* classInfo() const { return &s_info; }
    static const JSC::ClassInfo s_info;

    static PassRefPtr<JSC::Structure> createStructure(JSC::JSValue prototype)
    {
        return JSC::Structure::create(prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), AnonymousSlotCount);
    }

    static JSC::JSValue getConstructor(JSC::ExecState*, JSC::JSGlobalObject*);

    // Custom attributes
    JSC::JSValue value(JSC::ExecState*) const;

    // Custom functions
    JSC::JSValue convertToSpecifiedUnits(JSC::ExecState*, const JSC::ArgList&);
    JSSVGPODTypeWrapper<SVGLength> * impl() const { return m_impl.get(); }

private:
    RefPtr<JSSVGPODTypeWrapper<SVGLength> > m_impl;
protected:
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | Base::StructureFlags;
};

JSC::JSValue toJS(JSC::ExecState*, JSDOMGlobalObject*, JSSVGPODTypeWrapper<SVGLength>*, SVGElement*);
SVGLength toSVGLength(JSC::JSValue);

class JSSVGLengthPrototype : public JSC::JSObject {
    typedef JSC::JSObject Base;
public:
    static JSC::JSObject* self(JSC::ExecState*, JSC::JSGlobalObject*);
    virtual const JSC::ClassInfo* classInfo() const { return &s_info; }
    static const JSC::ClassInfo s_info;
    virtual bool getOwnPropertySlot(JSC::ExecState*, const JSC::Identifier&, JSC::PropertySlot&);
    virtual bool getOwnPropertyDescriptor(JSC::ExecState*, const JSC::Identifier&, JSC::PropertyDescriptor&);
    static PassRefPtr<JSC::Structure> createStructure(JSC::JSValue prototype)
    {
        return JSC::Structure::create(prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), AnonymousSlotCount);
    }
    JSSVGLengthPrototype(NonNullPassRefPtr<JSC::Structure> structure) : JSC::JSObject(structure) { }
protected:
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | Base::StructureFlags;
};

// Functions

JSC::JSValue JSC_HOST_CALL jsSVGLengthPrototypeFunctionNewValueSpecifiedUnits(JSC::ExecState*, JSC::JSObject*, JSC::JSValue, const JSC::ArgList&);
JSC::JSValue JSC_HOST_CALL jsSVGLengthPrototypeFunctionConvertToSpecifiedUnits(JSC::ExecState*, JSC::JSObject*, JSC::JSValue, const JSC::ArgList&);
// Attributes

JSC::JSValue jsSVGLengthUnitType(JSC::ExecState*, JSC::JSValue, const JSC::Identifier&);
JSC::JSValue jsSVGLengthValue(JSC::ExecState*, JSC::JSValue, const JSC::Identifier&);
void setJSSVGLengthValue(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsSVGLengthValueInSpecifiedUnits(JSC::ExecState*, JSC::JSValue, const JSC::Identifier&);
void setJSSVGLengthValueInSpecifiedUnits(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsSVGLengthValueAsString(JSC::ExecState*, JSC::JSValue, const JSC::Identifier&);
void setJSSVGLengthValueAsString(JSC::ExecState*, JSC::JSObject*, JSC::JSValue);
JSC::JSValue jsSVGLengthConstructor(JSC::ExecState*, JSC::JSValue, const JSC::Identifier&);
// Constants

JSC::JSValue jsSVGLengthSVG_LENGTHTYPE_UNKNOWN(JSC::ExecState*, JSC::JSValue, const JSC::Identifier&);
JSC::JSValue jsSVGLengthSVG_LENGTHTYPE_NUMBER(JSC::ExecState*, JSC::JSValue, const JSC::Identifier&);
JSC::JSValue jsSVGLengthSVG_LENGTHTYPE_PERCENTAGE(JSC::ExecState*, JSC::JSValue, const JSC::Identifier&);
JSC::JSValue jsSVGLengthSVG_LENGTHTYPE_EMS(JSC::ExecState*, JSC::JSValue, const JSC::Identifier&);
JSC::JSValue jsSVGLengthSVG_LENGTHTYPE_EXS(JSC::ExecState*, JSC::JSValue, const JSC::Identifier&);
JSC::JSValue jsSVGLengthSVG_LENGTHTYPE_PX(JSC::ExecState*, JSC::JSValue, const JSC::Identifier&);
JSC::JSValue jsSVGLengthSVG_LENGTHTYPE_CM(JSC::ExecState*, JSC::JSValue, const JSC::Identifier&);
JSC::JSValue jsSVGLengthSVG_LENGTHTYPE_MM(JSC::ExecState*, JSC::JSValue, const JSC::Identifier&);
JSC::JSValue jsSVGLengthSVG_LENGTHTYPE_IN(JSC::ExecState*, JSC::JSValue, const JSC::Identifier&);
JSC::JSValue jsSVGLengthSVG_LENGTHTYPE_PT(JSC::ExecState*, JSC::JSValue, const JSC::Identifier&);
JSC::JSValue jsSVGLengthSVG_LENGTHTYPE_PC(JSC::ExecState*, JSC::JSValue, const JSC::Identifier&);

} // namespace WebCore

#endif // ENABLE(SVG)

#endif
