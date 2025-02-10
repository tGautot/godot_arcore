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

    void process(ArSession &p_ar_session, ArFrame &p_ar_frame);

	ArAugmentedImageDatabase* getActiveDatabase() {return image_db;}

    int registerImageForTracking(ArSession *p_ar_session, godot::Image &p_img, const char *p_img_name);

    ArAnchor* getImageAnchor(int p_img_name);

private:
    ArAugmentedImageDatabase *image_db = nullptr;
    godot::HashMap<int, ArAnchor*> img_to_anchor;
};



}


#endif // ARCORE_PLUGIN_IMAGE_TRACKER
