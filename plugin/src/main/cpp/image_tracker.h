#ifndef ARCORE_PLUGIN_IMAGE_TRACKER
#define ARCORE_PLUGIN_IMAGE_TRACKER

#include "include/arcore_c_api.h"
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/variant/typed_dictionary.hpp>
#include <godot_cpp/templates/hash_map.hpp>


namespace arcore_plugin {


class ImageTracker {
public:
    ImageTracker();
    ~ImageTracker();


    void initialize(ArSession *p_ar_session);
    void createFreshDatabase(ArSession *p_ar_session);
    void serializeDatabase(ArSession *p_ar_session, uint8_t **r_byte_array, int64_t *r_size);
    void loadSerializedDatabase(ArSession *p_ar_session, const uint8_t *r_byte_array, int64_t r_size);
    void getImageTransformMatrix(ArSession *p_ar_session, const godot::String &p_img_name, float* r_mat4);

    void process(ArSession &p_ar_session, ArFrame &p_ar_frame);

	ArAugmentedImageDatabase* getActiveDatabase() {return image_db;}

    int registerImageForTracking(ArSession *p_ar_session, godot::Image *p_img, const godot::String &p_img_name, float p_phys_img_width = -1.0);

    ArAnchor* getImageAnchor(const godot::String &p_img_name);

private:
    ArAugmentedImageDatabase *image_db = nullptr;
    godot::HashMap<godot::String, ArAnchor*> name_to_anchor;
};



}


#endif // ARCORE_PLUGIN_IMAGE_TRACKER
