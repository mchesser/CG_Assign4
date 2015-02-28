use nalgebra::{Vec3, Rot3, Mat4, Iso3};
use nalgebra::{Zero, ToHomogeneous};

pub struct Camera {
    position: Vec3<f32>,
    pitch: f32,
    yaw: f32,
}

pub const UP: Vec3<f32> = Vec3 { x: 0.0, y: 1.0, z: 0.0 };
pub const DOWN: Vec3<f32> = Vec3 { x: 0.0, y: -1.0, z: 0.0 };
pub const LEFT: Vec3<f32> = Vec3 { x: -1.0, y: 0.0, z: 0.0 };
pub const RIGHT: Vec3<f32> = Vec3 { x: 1.0, y: 0.0, z: 0.0 };
pub const FORWARD: Vec3<f32> = Vec3 { x: 0.0, y: 0.0, z: 1.0 };
pub const BACK: Vec3<f32> = Vec3 { x: 0.0, y: 0.0, z: -1.0 };

impl Camera {
    pub fn new() -> Camera {
        Camera {
            position: Vec3::zero(),
            pitch: 0.0,
            yaw: 0.0,
        }
    }

    pub fn move_by(&mut self, amount: f32, direction: Vec3<f32>) {
        self.position = self.position + self.rotation_mat() * (direction * amount);
    }

    pub fn rotate(&mut self, pitch: f32, yaw: f32) {
        self.pitch += pitch;
        self.yaw += yaw;
    }

    pub fn to_mat(&self) -> Mat4<f32> {
        Iso3::new_with_rotmat(self.position, self.rotation_mat()).to_homogeneous()
    }

    fn rotation_mat(&self) -> Rot3<f32> {
        Rot3::new_with_euler_angles(0.0, self.pitch, self.yaw)
    }
}
