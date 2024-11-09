const std = @import("std");
const fs = std.fs;

pub fn build(b: *std.Build) !void {
    // Cross-compile to Windows
    const target = b.resolveTargetQuery(.{
        .cpu_arch = .x86_64,
        .os_tag = .windows,
    });

    // Get optimization options from the build arguments
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "main",
        .target = target,
        .optimize = optimize,
    });

    // Get all source files
    var source_files = std.ArrayList([]const u8).init(b.allocator);
    defer {
        for (source_files.items) |item| {
            b.allocator.free(item);
        }
        source_files.deinit();
    }

    const src = "src";
    var iter_dir = try std.fs.cwd().openDir(src, .{ .iterate = true });
    defer iter_dir.close();

    var iter = iter_dir.iterate();
    while (try iter.next()) |entry| {
        if (entry.kind == .file and std.mem.endsWith(u8, entry.name, "cpp")) {
            const file = try std.fs.path.join(b.allocator, &.{ src, entry.name });
            try source_files.append(file);
        }
    }

    exe.addCSourceFiles(.{
        .files = source_files.items,
        .flags = &.{ "-Werror", "-Wall", "-Wextra", "-std=c++20", "-pedantic" },
    });

    // Link with system libraries
    exe.linkSystemLibrary("winmm");
    exe.linkSystemLibrary("gdi32");
    exe.linkSystemLibrary("gdiplus");
    exe.linkLibCpp();

    exe.subsystem = .Windows;

    // Build the program
    b.installArtifact(exe);

    // Run the program
    const run_exe = b.addRunArtifact(exe);
    const run_step = b.step("run", "Run the application");
    run_step.dependOn(&run_exe.step);
}