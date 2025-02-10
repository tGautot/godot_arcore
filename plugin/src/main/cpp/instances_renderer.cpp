#include "instances_renderer.h"
#include "utils.h"
#include "ar_utils.h"
#include <godot_cpp/classes/camera_server.hpp>

using namespace arcore_plugin;
using namespace godot;

InstancesRenderer::InstancesRenderer() {
}

InstancesRenderer::~InstancesRenderer() {
}

void InstancesRenderer::process(ArSession &p_ar_session) {
    for(HashMap<size_t, Node3D*>::Iterator i = m_instances_nodes.begin();
            i != m_instances_nodes.end(); ++i){
        ALOGV("Godot ARCore: InstanceRenderer process\n");
        int k =  i->key;
        //ALOGV("Godot ARCore: looking at image with id %d\n", k);
        Node3D* v = i->value;

        ArAnchor* anchor = m_image_tracker->getImageAnchor(k);
        if(anchor == nullptr){
            //v->set_visible(false);
            continue;
        } 
        //v->set_visible(true);
        
        ScopedArPose scopedArPose(&p_ar_session);
        ArAnchor_getPose(&p_ar_session, anchor, scopedArPose.GetArPose());

        float mat4[16];
		ArPose_getMatrix(&p_ar_session, scopedArPose.GetArPose(), mat4);
		
        Vector3 nodePos = to_godot_position(mat4);
        //Transform3D transform = to_godot_transform(mat4);


        v->set_position(nodePos);
    }
    
}

void InstancesRenderer::clear() {
}

void InstancesRenderer::set_node_images_mapping(ArSession *p_ar_session, const Dictionary &in_nodeImagesMap) {
	clear();
	m_nodeImagesMap = in_nodeImagesMap;

    Array allImages = m_nodeImagesMap.keys();
    int64_t nImages = allImages.size();
    for(int i = 0; i < nImages; i++){
        Variant image_vrt = allImages.get(i);
        //Variant::Type img_type = image_vrt.get_type();
        //ERR_FAIL_COND_MSG(img_type != Variant::Type::OBJECT, "Dictionary key is expected to be of type godot::Image, but is not even an object"); 
        //Object* img_obj = image_vrt;
        Image *img = Object::cast_to<Image>(image_vrt.get_validated_object());
        ERR_FAIL_NULL_MSG(img, "Dictionary key cannot be cast to an image");

        Node3D *node = Object::cast_to<Node3D>(m_nodeImagesMap.get(image_vrt, Variant(false)));
        ERR_FAIL_NULL_MSG(node, "Dictionary value cannot be cast to Node3D");
        const char* nodeName = String(node->get_name()).ascii().ptr();

        // The following call takes a long time (20-30ms)  
        int trackerId = m_image_tracker->registerImageForTracking(p_ar_session, *img, nodeName);
        m_instances_nodes.insert(trackerId, node);
    }
}

//ALOGV("Godot ARCore: Verbose: Image %d of instancerender is variant of type %s\n", i, Variant::get_type_name(image_vrt.get_type()).ascii().ptr());