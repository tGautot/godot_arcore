extends Node3D

var _plugin_name = "ARCorePlugin"
var _android_plugin

var is_estimating_light = false
var original_light_trsf: Transform3D;

@export var skyCamMaterial: ShaderMaterial
var camFeed: CameraFeed

# Called when the node enters the scene tree for the first time.
func _enter_tree() -> void:
	for s in Engine.get_singleton_list():
		print("MCT " + s.get_basename())
	if Engine.has_singleton(_plugin_name):
		print("MCT found plugin")
		_android_plugin = Engine.get_singleton(_plugin_name)
	else:
		printerr("MCT Couldn't find plugin " + _plugin_name)

	print("MCT before initialize_wrapper")
	_android_plugin.initializeEnvironment()

	print("MCT called initialize_wrapper")

	var ar_interface = ARCoreInterfaceInstance.get_interface()
	var interface_name = ar_interface.get_name()
	print("MCT " + interface_name)

	ARCoreInterfaceInstance.start()

	original_light_trsf = $DirectionalLight3D.transform
	
	ARCoreInterfaceInstance.enable_images_detection(true)
	ARCoreInterfaceInstance.create_image_tracker_database()
	
	camFeed = ARCoreInterfaceInstance.get_camera_feed()
	if camFeed:
		_setup_skyshader_camera_params()
	else:
		print("NO CAMERA FEED ON ENTER TREE")
	#ARCoreInterfaceInstance.enable_vertical_plane_detection(true)
	#ARCoreInterfaceInstance.enable_horizontal_plane_detection(true)

	#ARCoreInterfaceInstance.image_tracker_database_add_image(imageToTrack.get_image(), "base")
	#ARCoreInterfaceInstance.track_nodes_to_images({
	#	imageToTrack.get_image(): nodeToStick
	#})

func _exit_tree():
	_android_plugin.uninitializeEnvironment()

func _setup_skyshader_camera_params():
	var ytex: CameraTexture = skyCamMaterial.get_shader_parameter("camera_y")
	var ctex: CameraTexture = skyCamMaterial.get_shader_parameter("camera_CbCr")
	ytex.camera_feed_id = camFeed.get_id()
	ctex.camera_feed_id = camFeed.get_id()
	print("Setting up camera shader, id is ", camFeed.get_id())
	skyCamMaterial.set_shader_parameter("camera_y", ytex)
	skyCamMaterial.set_shader_parameter("camera_CbCr", ctex)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	
	if camFeed == null:
		print("STILL NO CAMERA FEED")
		camFeed = ARCoreInterfaceInstance.get_camera_feed()
		if camFeed:
			_setup_skyshader_camera_params()
			
	if camFeed:
		print("Camera feed transform: ", camFeed.feed_transform)
		skyCamMaterial.set_shader_parameter("feed_transform_basis_x", camFeed.feed_transform.x)
		skyCamMaterial.set_shader_parameter("feed_transform_basis_y", camFeed.feed_transform.y)
		skyCamMaterial.set_shader_parameter("feed_transform_origin", camFeed.feed_transform.origin)
		var vpSize = get_viewport().size
		skyCamMaterial.set_shader_parameter("viewport_size", vpSize)
	#var imgTrsfm: Transform3D = ARCoreInterfaceInstance.image_tracker_get_tracked_transform("base")
	#nodeToStick.position = imgTrsfm.origin
	if (is_estimating_light):
		var light_dir = ARCoreInterfaceInstance.get_light_main_hdr_direction()
		$DirectionalLight3D.rotation = light_dir
		var intensity = ARCoreInterfaceInstance.get_light_main_hdr_intensity()
		$DirectionalLight3D.light_intensity_lumens = intensity.x;
	else:
		$DirectionalLight3D.transform = original_light_trsf


func _on_node_2d_estimate_depthmap_toggled(toggled):
	ARCoreInterfaceInstance.enable_depth_estimation(toggled)

func _on_node_2d_show_depthmap_toggled(toggled):
	ARCoreInterfaceInstance.show_depth_map(toggled)

func _on_node_2d_far_changed(value):
	ARCoreInterfaceInstance.set_max_depth_meters(value)

func _on_node_2d_vertical_planes_toggled(toggled):
	ARCoreInterfaceInstance.enable_vertical_plane_detection(toggled)

func _on_node_2d_horizontal_planes_toggled(toggled):
	ARCoreInterfaceInstance.enable_horizontal_plane_detection(toggled)

func _on_node_2d_images_detection_toggled(toggled):
	ARCoreInterfaceInstance.enable_images_detection(toggled)

func _on_node_2d_instant_placement_toggled(toggled):
	ARCoreInterfaceInstance.enable_instant_placement(toggled)

func _on_node_2d_light_estimation_toggled(toggled):
	is_estimating_light = toggled
	ARCoreInterfaceInstance.enable_light_estimation(toggled)

func _on_node_2d_point_cloud_detection_toggled(toggled):
	ARCoreInterfaceInstance.enable_point_cloud_detection(toggled)

func _on_node_2d_switch_orientation(vertical):
	ARCoreInterfaceInstance.switch_orientation(vertical)


func _on_screen_touch_down(pos: Vector2) -> void:
	#print("Detected click at ", pos)
	#var cam = get_viewport().get_camera_3d()
	#var orig = cam.project_ray_origin(pos)
	#var dir = cam.project_ray_normal(pos)
	#print("Resulting ray is ", orig, dir)
	#
	#var trsfm = ARCoreInterfaceInstance.raycast(orig, dir)
	var trsfm = ARCoreInterfaceInstance.screenRayCast(pos.x, pos.y)
	print("Resulting raycast hit transform is ", trsfm)
