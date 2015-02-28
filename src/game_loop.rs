use std::old_io::timer;
use std::time::duration::Duration;

use glutin::{Event, VirtualKeyCode, ElementState};
use glium::Display;

use game::Game;
use renderer::Renderer;
use clock_ticks;

const FIXED_TIME_STEP: bool = true;
const TARGET_TIME_STEP: u64 = 16666667;

pub fn run(display: Display, mut game: Game) {
    let mut renderer = Renderer::init(&display);

    let mut prev_time = clock_ticks::precise_time_ns();
    let mut frame_time = 0;

    'main: loop {
        renderer.draw(&game.camera);

        for event in display.poll_events() {
            match event {
                Event::KeyboardInput(_, _, Some(VirtualKeyCode::Escape)) => break 'main,
                Event::KeyboardInput(ElementState::Pressed, _, Some(key)) => game.keydown(key),
                Event::KeyboardInput(ElementState::Released, _, Some(key)) => game.keyup(key),
                Event::Resized(width, height) => renderer.resize(width, height),
                Event::Closed => break 'main,
                _ => {},
            }
        }

        let current_time = clock_ticks::precise_time_ns();
        if FIXED_TIME_STEP {
            frame_time += current_time - prev_time;
            while frame_time >= TARGET_TIME_STEP {
                game.update(TARGET_TIME_STEP as f64 / 1000000.0);
                frame_time -= TARGET_TIME_STEP;
            }
            timer::sleep(Duration::nanoseconds((TARGET_TIME_STEP - frame_time) as i64));
        }
        else {
            game.update((current_time - prev_time) as f64 / 1000000.0);
        }

        prev_time = current_time;
    }
}
