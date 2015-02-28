#![feature(plugin, old_io, core, std_misc)]
#![plugin(glium_macros)]

extern crate glutin;
#[macro_use] extern crate glium;

extern crate nalgebra;
extern crate clock_ticks;

use glium::DisplayBuild;

mod shaders;
mod scene;
mod camera;
mod renderer;
mod game;
mod game_loop;

fn main() {
    let display = glutin::WindowBuilder::new()
        .with_dimensions(800, 600)
        .with_title(format!("Random City"))
        .build_glium()
        .unwrap();

    let game = game::Game::init();
    game_loop::run(display, game);
}
