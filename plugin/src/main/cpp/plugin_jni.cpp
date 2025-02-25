#include <jni.h>

#include <godot_cpp/variant/utility_functions.hpp>

#include "utils.h"

#include "arcore_wrapper.h"

#undef JNI_PACKAGE_NAME
// TODO: Update to match plugin's package name
#define JNI_PACKAGE_NAME org_godotengine_plugin_android_arcore

#undef JNI_CLASS_NAME
#define JNI_CLASS_NAME ARCorePlugin

extern "C" {
JNIEXPORT void JNICALL JNI_METHOD(initializeEnvironment)(JNIEnv *env, jobject activity) {
	ARCoreWrapper::initialize_environment(env, activity);
}
JNIEXPORT void JNICALL JNI_METHOD(uninitializeEnvironment)(JNIEnv *env, jobject) {
	ARCoreWrapper::uninitialize_environment(env);
}
JNIEXPORT void JNICALL JNI_METHOD(onDisplayGeometryChanged)(JNIEnv *env, jobject, int display_orientation, int display_width, int display_height){
	ARCoreWrapper::on_display_geometry_changed(display_orientation, display_width, display_height);
	ARCoreWrapper::set_viewport_changed(true);
}
};
