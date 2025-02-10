#include "image_tracker.h"

#include "include/arcore_c_api.h"
#include <godot_cpp/classes/image.hpp>

#include "utils.h"
#include "ar_utils.h"
using namespace arcore_plugin;
using namespace godot;


ImageTracker::ImageTracker(){
}

ImageTracker::~ImageTracker(){
    if(image_db != nullptr){
        ArAugmentedImageDatabase_destroy(image_db);
    }
}

void ImageTracker::initialize(ArSession *p_ar_session){
    ALOGV("Godot ARCore: Initializing ImageTracker\n");
    ArAugmentedImageDatabase_create(p_ar_session, &image_db);
}

void ImageTracker::process(ArSession &p_ar_session, ArFrame &p_ar_frame){
    ArTrackableList* updated_image_list = nullptr;
    ArTrackableList_create(&p_ar_session, &updated_image_list);
    ArFrame_getUpdatedTrackables(
			&p_ar_session, &p_ar_frame, AR_TRACKABLE_AUGMENTED_IMAGE, updated_image_list);

    int32_t image_list_size;
    ArTrackableList_getSize(&p_ar_session, updated_image_list, &image_list_size);
    int32_t totImg;
    ArAugmentedImageDatabase_getNumImages(&p_ar_session, image_db, &totImg);
    ALOGV("Currently tracking a total of %d/%d images\n", image_list_size, totImg);
    for (int i = 0; i < image_list_size; ++i) {
        ArTrackable* ar_trackable = nullptr;
        ArTrackableList_acquireItem(&p_ar_session, updated_image_list, i,
                                    &ar_trackable);
        ArAugmentedImage* image = ArAsAugmentedImage(ar_trackable);

        ArTrackingState tracking_state;
        ArTrackable_getTrackingState(&p_ar_session, ar_trackable, &tracking_state);

        int image_index;
        ArAugmentedImage_getIndex(&p_ar_session, image, &image_index);

        if (tracking_state == AR_TRACKING_STATE_TRACKING) {
            ALOGV("Godot ARCore: Tracked Image index %d is being tracked\n", image_index);
            ScopedArPose scopedArPose(&p_ar_session);
            ArAugmentedImage_getCenterPose(&p_ar_session, image,
                                        scopedArPose.GetArPose());

            ArAnchor* image_anchor = nullptr;
            const ArStatus status = ArTrackable_acquireNewAnchor(
					&p_ar_session, ar_trackable, scopedArPose.GetArPose(), &image_anchor);

			img_to_anchor[image_index] = image_anchor;
        }
        else{
            if(tracking_state == AR_TRACKING_STATE_PAUSED){
                ALOGV("Godot ARCore: Tracked Image index %d has tracking PAUSED\n", image_index);
            }
            if(tracking_state == AR_TRACKING_STATE_STOPPED){
                ALOGV("Godot ARCore: Tracked Image index %d has tracking STOPPED\n", image_index);
            }
            //img_to_anchor[image_index] = nullptr;
        }
    }
}


int ImageTracker::registerImageForTracking(ArSession *p_ar_session, godot::Image &p_img, const char * p_img_name){

    // TODO put this in background thread as said here
    // https://developers.google.com/ar/reference/c/group/ar-augmented-image-database#araugmentedimagedatabase_addimage

    int32_t w = p_img.get_width(), h = p_img.get_height();
    ALOGV("Godot ARCore: Registering %dx%d image\n", w, h);
    ALOGV("Godot ARCore: Session null: %d   - Image DB null: %d\n", p_ar_session == nullptr, image_db == nullptr);
    uint8_t* grayscale = (uint8_t*) memalloc(w*h*sizeof(uint8_t));

    //String gsAsString = "";

    for(int j = 0; j < h; j++){
    for(int i = 0; i < w; i++){
        Color pixc = p_img.get_pixel(i, j);
        grayscale[i+j*w] = uint8_t(CLAMP( 255.0*(0.299*pixc.r + 0.587*pixc.g + 0.115*pixc.b), 0, 255));
        //gsAsString += grayscale[i+j*w] + " ";
    } /*gsAsString += "\n";*/ }

    //ALOGV("Godot ARCore: Image grayscale is \n%s\n", gsAsString.ascii().ptr());

    int32_t outidx;

    ArStatus stts = ArAugmentedImageDatabase_addImageWithPhysicalSize(p_ar_session, image_db, p_img_name, grayscale, w, h, w, 0.14, &outidx);

    img_to_anchor.insert(outidx, nullptr);

    memfree(grayscale);

    if(stts == AR_SUCCESS){
        ALOGV("Godot ARCore: Registered tracked image, id is %d\n", outidx);
        return outidx;
    }

    if(stts == AR_ERROR_INVALID_ARGUMENT){
        ALOGE("Godot ARCore: Invalid arguments in addImage (stride != width?)");
    }

    if(stts == AR_ERROR_IMAGE_INSUFFICIENT_QUALITY){
        ALOGE("Godot ARCore: Cannot track image %s , lacking trackable features", p_img_name);
    }

    return stts;

}

ArAnchor* ImageTracker::getImageAnchor(int p_img_id){
    return img_to_anchor.get(p_img_id);
}
