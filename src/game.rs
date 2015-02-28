use glutin::VirtualKeyCode;
use camera::{self, Camera};

#[derive(PartialEq)]
pub struct MoveState {
    pub forwards: bool,
    pub backwards: bool,
    pub left: bool,
    pub right: bool,
}

pub struct Game {
    pub camera: Camera,
    pub move_state: MoveState,
}

impl Game {
    pub fn init() -> Game {
        Game {
            camera: Camera::new(),
            move_state: MoveState {
                forwards: false,
                backwards: false,
                left: false,
                right: false,
            },
        }
    }

    pub fn update(&mut self, dt: f64) {
        update_camera(&mut self.camera, &self.move_state, dt);
    }

    pub fn keydown(&mut self, key: VirtualKeyCode) {
        match key {
            VirtualKeyCode::W => self.move_state.forwards = true,
            VirtualKeyCode::A => self.move_state.left = true,
            VirtualKeyCode::S => self.move_state.backwards = true,
            VirtualKeyCode::D => self.move_state.right = true,
            _ => {},
        }
    }

    pub fn keyup(&mut self, key: VirtualKeyCode) {
        match key {
            VirtualKeyCode::W => self.move_state.forwards = false,
            VirtualKeyCode::A => self.move_state.left = false,
            VirtualKeyCode::S => self.move_state.backwards = false,
            VirtualKeyCode::D => self.move_state.right = false,
            _ => {},
        }
    }
}


fn update_camera(camera: &mut Camera, move_state: &MoveState, dt: f64) {
    let speed = 0.01;
    if move_state.forwards { camera.move_by(speed * dt as f32, camera::FORWARD); }
    if move_state.left { camera.move_by(speed * dt as f32, camera::LEFT); }
    if move_state.backwards { camera.move_by(speed * dt as f32, camera::BACK); }
    if move_state.right { camera.move_by(speed * dt as f32, camera::RIGHT); }
}
