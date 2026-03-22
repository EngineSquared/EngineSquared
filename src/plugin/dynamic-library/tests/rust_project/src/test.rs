use std::ffi::c_void;

#[repr(C)]
pub struct MetaCore {
    pub context: *mut c_void,
    pub get_component_id: extern "C" fn(*mut MetaCore, *const i8) -> u32,
    pub create_view: extern "C" fn() -> *mut c_void,
    pub destroy_view: extern "C" fn(*mut c_void),
    pub aggregate_component_to_view: extern "C" fn(*mut MetaCore, *mut c_void, u32),
    pub view_for_each:
        extern "C" fn(*mut MetaCore, *mut c_void, *mut c_void, extern "C" fn(u32, *mut c_void)),
    pub get_entity_component_from_view: extern "C" fn(*mut MetaCore, u32, u32) -> *mut c_void,
    pub add_system: extern "C" fn(*mut MetaCore, extern "C" fn(*mut MetaCore)),
}

#[repr(C)]
pub struct RustPosition {
    pub x: f32,
    pub y: f32,
}

extern "C" fn view_for_each_callback(entity_id: u32, user_data: *mut c_void) {
    if user_data.is_null() {
        println!("User data is null");
        return;
    }

    let core = user_data.cast::<MetaCore>();

    // SAFETY: The engine passes a valid MetaCore pointer as user_data in ViewForEach.
    unsafe {
        println!("Entity ID: {entity_id}, User Data: {}", core as usize);

        let component_name = b"RustPosition\0";
        let component_id = ((*core).get_component_id)(core, component_name.as_ptr().cast());
        let position_ptr = ((*core).get_entity_component_from_view)(core, component_id, entity_id)
            .cast::<RustPosition>();

        if position_ptr.is_null() {
            println!("Entity {entity_id} does not have RustPosition component");
            return;
        }

        let position = &mut *position_ptr;
        println!("RustPosition: x={}, y={}", position.x, position.y);

        if position.x == 69.0 {
            position.x = 0.0;
        } else if position.x == 67.0 {
            position.x = 2.0;
        }

        if position.y == 42.0 {
            position.y = 1.0;
        } else if position.y == 123_456_789.0 {
            position.y = 3.0;
        }
    }
}

#[no_mangle]
pub extern "C" fn system(core: *mut MetaCore) {
    if core.is_null() {
        println!("Core is null");
        return;
    }

    println!("------------------------");
    println!("Hello from Rust!");

    // SAFETY: `core` is validated above and provided by the host engine.
    unsafe {
        let component_name = b"RustPosition\0";
        let component_id = ((*core).get_component_id)(core, component_name.as_ptr().cast());
        if component_id == 0 {
            println!("Component 'RustPosition' not found!");
            return;
        }

        println!("Component ID for RustPosition: {component_id}");

        let view = ((*core).create_view)();
        ((*core).aggregate_component_to_view)(core, view, component_id);
        ((*core).view_for_each)(core, view, core.cast::<c_void>(), view_for_each_callback);
        ((*core).destroy_view)(view);
    }
}

#[no_mangle]
pub extern "C" fn plugin_bind(core: *mut MetaCore) {
    if core.is_null() {
        println!("Core is null");
        return;
    }

    // SAFETY: `core` is validated above and provided by the host engine.
    unsafe {
        ((*core).add_system)(core, system);
    }
}
