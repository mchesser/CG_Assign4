use std::default::Default;

use glium::{Surface, Display};
use glium::{VertexBuffer, IndexBuffer, Program};
use glium::{DrawParameters, BackfaceCullingMode, DepthTest};
use glium::index::TrianglesList;

use nalgebra::PerspMat3;

use shaders::Vertex;
use camera::Camera;
use scene::build_city;

pub struct Renderer<'a> {
    vertex_buffer: VertexBuffer<Vertex>,
    index_buffer: IndexBuffer,
    shader: Program,
    display: &'a Display,
    draw_params: DrawParameters,
    persp_mat: PerspMat3<f32>,
}

impl<'a> Renderer<'a> {
    pub fn init(display: &'a Display) -> Renderer<'a> {
        let scene = build_city();
        Renderer {
            vertex_buffer: VertexBuffer::new(&display, scene.vertex_buffer),
            index_buffer: IndexBuffer::new(&display, TrianglesList(scene.index_buffer)),
            shader: {
                Program::from_source(&display, include_str!("shaders/shader.v.glsl"),
                    include_str!("shaders/shader.f.glsl"), None).unwrap()
            },
            display: &display,
            draw_params: DrawParameters {
                depth_test: DepthTest::IfLess,
                depth_write: true,
                backface_culling: BackfaceCullingMode::CullCounterClockWise,
                .. DrawParameters::default()
            },
            persp_mat: PerspMat3::new(800.0 / 600.0, 45.0, 0.1, 1000.0),
        }
    }

    pub fn draw(&self, camera: &Camera) {
        let uniforms = uniform! {
            matrix: self.persp_mat.to_mat() * camera.to_mat()
        };

        let mut frame = self.display.draw();
        frame.clear(Some((0.0, 0.0, 0.0, 0.0)), Some(1000.0), None);
        frame.draw(&self.vertex_buffer, &self.index_buffer, &self.shader, &uniforms,
            &self.draw_params).unwrap();
        frame.finish();
    }

    pub fn resize(&mut self, width: u32, height: u32) {
        self.persp_mat = PerspMat3::new(width as f32 / height as f32, 45.0, 0.1, 1000.0);
    }
}
