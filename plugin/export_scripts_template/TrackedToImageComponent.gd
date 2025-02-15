extends Node

var target: Node3D

@export var trackedImage: Image

@export_group("Transform")
@export var position_offset: Vector3 = Vector3(0,0,0)

@export var disabled: bool = false

enum ROTATION_MIX {ABSOLUTE, RELATIVE}

@export var x_rotation_mix: ROTATION_MIX = ROTATION_MIX.RELATIVE
@export var x_rotation_val: float = 0.0

@export var y_rotation_mix: ROTATION_MIX = ROTATION_MIX.RELATIVE
@export var y_rotation_val: float = 0.0

@export var z_rotation_mix: ROTATION_MIX = ROTATION_MIX.RELATIVE
@export var z_rotation_val: float = 0.0

func _ready():
	target = get_parent()
	ARCoreInterfaceInstance.image_tracker_database_add_image(trackedImage, trackedImage.resource_name)

func _process(_delta: float):
	var trackedTrsfrm: Transform3D = ARCoreInterfaceInstance.image_tracker_get_tracked_transform(trackedImage.resource_name)
	target.position = trackedTrsfrm.origin + position_offset
	
	var imgRot = trackedTrsfrm.basis.get_euler()
	
	var rot_x = x_rotation_val
	if x_rotation_mix == ROTATION_MIX.RELATIVE:
		rot_x += imgRot.x
	target.rotation.x = rot_x
	
	
	var rot_y = y_rotation_val
	if y_rotation_mix == ROTATION_MIX.RELATIVE:
		rot_y += imgRot.y
	target.rotation.y = rot_y
	
	
	var rot_z = z_rotation_val
	if z_rotation_mix == ROTATION_MIX.RELATIVE:
		rot_z += imgRot.z
	target.rotation.z = rot_z
