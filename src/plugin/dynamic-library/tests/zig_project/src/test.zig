const std = @import("std");

const MetaCore = extern struct {
    context: *void,
    GetComponentId: *const fn (*MetaCore, [*c]const u8) callconv(.c) u32,
    CreateView: *const fn () callconv(.c) *void,
    DestroyView: *const fn (*void) callconv(.c) void,
    AggregateComponentToView: *const fn (*MetaCore, *void, u32) callconv(.c) void,
    ViewForEach: *const fn (*MetaCore, *void, ?*anyopaque, *const fn (u32, *anyopaque) callconv(.c) void) callconv(.c) void,
    GetEntityComponentFromView: *const fn (*MetaCore, u32, u32) callconv(.c) *void,
    AddSystem: *const fn (*MetaCore, *const fn (*MetaCore) callconv(.c) void) callconv(.c) void,
};

const Position = extern struct {
    x: f32,
    y: f32,
};

fn viewForEachCallback(entityId: u32, userData: *anyopaque) callconv(.c) void {
    const core: *MetaCore = @ptrCast(@alignCast(userData));
    std.debug.print("Entity ID: {d}, User Data: {d}\n", .{ entityId, @intFromPtr(core) });
    const component_id = core.GetComponentId(core, "Position");
    var position: *Position = @ptrCast(@alignCast(core.GetEntityComponentFromView(core, component_id, entityId)));
    std.debug.print("Position: x={d}, y={d}\n", .{ position.x, position.y });
    if (position.x == 69) {
        position.x = 0;
    } else if (position.x == 67) {
        position.x = 2;
    }
    if (position.y == 42) {
        position.y = 1;
    } else if (position.y == 123456789.0) {
        position.y = 3;
    }
}

export fn system(core: *MetaCore) callconv(.c) void {
    std.debug.print("Hello from the System function!\n", .{});

    const component_id = core.GetComponentId(core, "Position");
    if (component_id == 0) {
        std.debug.print("Component 'Position' not found!\n", .{});
        return;
    }
    std.debug.print("Component ID for Position: {d}\n", .{component_id});

    const view = core.CreateView();
    defer core.DestroyView(view);

    core.AggregateComponentToView(core, view, component_id);

    core.ViewForEach(core, view, core, &viewForEachCallback);
}

export fn plugin_bind(core: *MetaCore) callconv(.c) void {
    core.AddSystem(core, &system);
}
