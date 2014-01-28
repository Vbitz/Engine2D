#include "JSMathHelper.hpp"

#include <math.h>

namespace Engine {
    namespace JsMathHelper {
        void InitMathHelper() {
            v8::HandleScope scp(v8::Isolate::GetCurrent());
            v8::Local<v8::Context> ctx = v8::Isolate::GetCurrent()->GetCurrentContext();
            v8::Context::Scope ctx_scope(ctx);
            
            v8::Local<v8::Object> obj = v8::Context::GetCurrent()->Global();
            
            v8::Local<v8::Object> math_table = v8::Object::Cast(*obj->Get(v8::String::New("Math")));
            
            math_table->Set(v8::String::NewSymbol("PI"), v8::Number::New(M_PI));
            math_table->Set(v8::String::NewSymbol("PI_2"), v8::Number::New(M_PI_2));
            math_table->Set(v8::String::NewSymbol("PI_4"), v8::Number::New(M_PI_4));
        }
    }
}