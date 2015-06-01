// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "JSIO.h"
#include "JSVM.h"
#include <Atomic/IO/File.h>

namespace Atomic
{

enum IO_MAGIC_TYPE
{
    IO_MAGIC_NONE = 0,
    IO_MAGIC_INT,
    IO_MAGIC_BOOL,
    IO_MAGIC_FLOAT,
    IO_MAGIC_STRING,
    IO_MAGIC_ZEROSTRING
};


static Serializer* CastToSerializer(duk_context* ctx, int index)
{
    Object* o = js_to_class_instance<Object>(ctx, index, 0);
    Serializer* serial = NULL;

    if (!o)
        return NULL;

    // type check! file supported for now
    if (o->GetType() == File::GetTypeStatic())
    {
        // cast it!
        serial = (Serializer*) ((File*)o);
    }

    return serial;

}

static Deserializer* CastToDeserializer(duk_context* ctx, int index)
{
    Object* o = js_to_class_instance<Object>(ctx, index, 0);
    Deserializer* deserial = NULL;

    if (!o)
        return NULL;

    // type check! file supported for now
    if (o->GetType() == File::GetTypeStatic())
    {
        // cast it!
        deserial = (Deserializer*) ((File*)o);
    }

    return deserial;

}


static int Serializer_Write(duk_context* ctx)
{
    duk_int_t magic = duk_get_current_magic(ctx);

    duk_push_this(ctx);

    // safe cast based on type check above
    Serializer* serial = CastToSerializer(ctx, duk_get_top_index(ctx));

    duk_pop(ctx);

    if (!serial)
    {
        duk_push_boolean(ctx, 0);
        return 1;
    }

    const char* str;
    size_t length;

    IO_MAGIC_TYPE v = (IO_MAGIC_TYPE) magic;

    bool success = false;

    switch(v)
    {
        case IO_MAGIC_INT:
            success = serial->WriteInt((int) duk_require_number(ctx, 0));
            break;
        case IO_MAGIC_STRING:
             str = duk_require_string(ctx, 0);
             length = strlen(str);
             success = serial->Write(str, length);
             /*
             if (length)
             {
                buffer.Resize(length);

                for (size_t i = 0; i < length; i++)
                    buffer[i] = str[i];

                serial->WriteBuffer(buffer);
             }
             */
            break;
        case IO_MAGIC_ZEROSTRING:
            success = serial->WriteString(duk_require_string(ctx, 0));
            break;
        default:
            break;
    }

    duk_push_boolean(ctx, success ? 1 : 0);

    return 1;

}

static int Deserializer_Read(duk_context* ctx)
{
    duk_int_t magic = duk_get_current_magic(ctx);

    duk_push_this(ctx);

    // safe cast based on type check above
    Deserializer* deserial = CastToDeserializer(ctx, duk_get_top_index(ctx));

    duk_pop(ctx);

    if (!deserial)
    {
        duk_push_boolean(ctx, 0);
        return 1;
    }

    PODVector<unsigned char> buffer;
    String str;
    size_t length;

    IO_MAGIC_TYPE v = (IO_MAGIC_TYPE) magic;

    bool success = false;

    switch(v)
    {
        case IO_MAGIC_INT:
            duk_push_number(ctx, (double) deserial->ReadInt());
            return 1;
        case IO_MAGIC_STRING:
            length = deserial->GetSize() - deserial->GetPosition();
            str.Resize(length + 1);
            deserial->Read(&str[0], length);
            str[length] = '\0';
            duk_push_string(ctx, str.CString());
            return 1;
        case IO_MAGIC_ZEROSTRING:
            success = duk_push_string(ctx, deserial->ReadString().CString());
            return 1;
        default:
            break;
    }

    duk_push_undefined(ctx);

    return 1;

}

static void AddSerializerMixin(duk_context* ctx, const String& package, const String& classname)
{
    js_class_get_prototype(ctx, package.CString(), classname.CString());

    duk_push_c_function(ctx, Serializer_Write, 1);
    duk_set_magic(ctx, -1, (unsigned) VAR_INT);
    duk_put_prop_string(ctx, -2, "writeInt");

    duk_push_c_function(ctx, Serializer_Write, 1);
    duk_set_magic(ctx, -1, (unsigned) IO_MAGIC_STRING);
    duk_put_prop_string(ctx, -2, "writeString");

    duk_push_c_function(ctx, Serializer_Write, 1);
    duk_set_magic(ctx, -1, (unsigned) IO_MAGIC_ZEROSTRING);
    duk_put_prop_string(ctx, -2, "writeZeroString");

    duk_pop(ctx);

}

static void AddDeserializerMixin(duk_context* ctx, const String& package, const String& classname)
{
    js_class_get_prototype(ctx, package.CString(), classname.CString());

    duk_push_c_function(ctx, Deserializer_Read, 0);
    duk_set_magic(ctx, -1, (unsigned) VAR_INT);
    duk_put_prop_string(ctx, -2, "readInt");

    duk_push_c_function(ctx, Deserializer_Read, 0);
    duk_set_magic(ctx, -1, (unsigned) IO_MAGIC_STRING);
    duk_put_prop_string(ctx, -2, "readString");

    duk_push_c_function(ctx, Deserializer_Read, 0);
    duk_set_magic(ctx, -1, (unsigned) IO_MAGIC_ZEROSTRING);
    duk_put_prop_string(ctx, -2, "readZeroString");

    duk_pop(ctx);

}


static int File_ReadText(duk_context* ctx)
{
    duk_push_this(ctx);
    File* file = js_to_class_instance<File>(ctx, -1, 0);

    String text;
    file->ReadText(text);

    duk_push_string(ctx, text.CString());

    return 1;
}

void jsapi_init_io(JSVM* vm)
{
    duk_context* ctx = vm->GetJSContext();

    js_class_get_prototype(ctx, "Atomic", "File");
    duk_push_c_function(ctx, File_ReadText, 0);
    duk_put_prop_string(ctx, -2, "readText");
    duk_pop(ctx);

    AddSerializerMixin(ctx, "Atomic", "File");
    AddDeserializerMixin(ctx, "Atomic", "File");

}

}
