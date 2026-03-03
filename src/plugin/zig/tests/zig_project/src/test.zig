const std = @import("std");

const MetaCore = extern struct {
    context: *void,
    GetComponentId: *const fn (*void, [*c]const u8) callconv(.c) u32,
    CreateView: *const fn () callconv(.c) *void,
    DestroyView: *const fn (*void) callconv(.c) void,
    AggregateComponentToView: *const fn (*void, *void, u32) callconv(.c) void,
    ViewForEach: *const fn (*void, *void, ?*anyopaque, *const fn (u32, *anyopaque) callconv(.c) void) callconv(.c) void,
    GetEntityComponentFromView: *const fn (*void, u32, u32) callconv(.c) *void,
    AddSystem: *const fn (*MetaCore, *const fn (*MetaCore) callconv(.c) void) callconv(.c) void,
};

const Position = extern struct {
    x: f32,
    y: f32,
};

fn viewForEachCallback(entityId: u32, userData: *anyopaque) callconv(.c) void {
    const core: *MetaCore = @ptrCast(@alignCast(userData));
    std.debug.print("Entity ID: {d}, User Data: {d}\n", .{ entityId, @intFromPtr(core) });
    const component_id = core.GetComponentId(core.context, "Position");
    const position: *Position = @ptrCast(@alignCast(core.GetEntityComponentFromView(core.context, component_id, entityId)));
    std.debug.print("Position: x={d}, y={d}\n", .{ position.x, position.y });
}

export fn system(core: *MetaCore) callconv(.c) void {
    // print something to the console to verify that the function is called correctly
    std.debug.print("Hello from the System function!\n", .{});

    const component_id = core.GetComponentId(core.context, "Position");
    std.debug.print("Component ID for Position: {d}\n", .{component_id});

    const view = core.CreateView();

    core.AggregateComponentToView(core.context, view, component_id);

    core.ViewForEach(core.context, view, core, &viewForEachCallback);

    core.DestroyView(view);
}

export fn plugin_bind(core: *MetaCore) callconv(.c) void {
    core.AddSystem(core, &system);
}
