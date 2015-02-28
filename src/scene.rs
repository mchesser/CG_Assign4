use std::slice;
use std::iter;

use nalgebra::{Vec3, Pnt3, Iso3, Mat3, Mat4, Zero, ToHomogeneous};
use shaders::Vertex;

static COLORS: [[f32; 3]; 8] = [
    [1.0, 0.0, 0.0],
    [0.0, 1.0, 0.0],
    [0.0, 0.0, 1.0],
    [1.0, 1.0, 0.0],
    [1.0, 0.0, 1.0],
    [0.0, 1.0, 1.0],
    [1.0, 1.0, 1.0],
    [0.5, 0.5, 0.5],
];

pub trait Model {
    type VertexIter: Iterator<Item=Pnt3<f32>>;
    type IndicesIter: Iterator<Item=u16>;

    fn vertices(&self) -> Self::VertexIter;
    fn indices(&self) -> Self::IndicesIter;
}

pub struct Scene {
    pub vertex_buffer: Vec<Vertex>,
    pub index_buffer: Vec<u16>,
}

impl Scene {
    pub fn new() -> Scene {
        Scene {
            vertex_buffer: vec![],
            index_buffer: vec![],
        }
    }

    pub fn add_model<M: Model>(&mut self, model: &M, transformation: Mat4<f32>) {
        let index_start = self.vertex_buffer.len() as u16;
        self.index_buffer.extend(model.indices().map(|i| i + index_start));

        let transform = |v: Pnt3<f32>| { transformation * v.to_homogeneous() };
        self.vertex_buffer.extend(model.vertices().enumerate()
            .map(|(i, v)| Vertex { position: *transform(v).as_array(), color: COLORS[i] }));
    }

    #[allow(dead_code)]
    pub fn clear(&mut self) {
        self.vertex_buffer.clear();
        self.index_buffer.clear();
    }
}

pub struct Cube {
    _blank: (),
}

impl Cube {
    pub fn new() -> Cube {
        Cube { _blank: () }
    }
}

impl Model for Cube {
    type VertexIter = iter::Cloned<slice::Iter<'static, Pnt3<f32>>>;
    type IndicesIter = iter::Cloned<slice::Iter<'static, u16>>;

    fn vertices(&self) -> <Cube as Model>::VertexIter {
        static CUBE_VERTICES: [Pnt3<f32>; 8] = [
            Pnt3 { x: -0.5, y:  0.5, z: -0.5 },
            Pnt3 { x:  0.5, y:  0.5, z: -0.5 },
            Pnt3 { x: -0.5, y:  0.5, z:  0.5 },
            Pnt3 { x:  0.5, y:  0.5, z:  0.5 },
            Pnt3 { x: -0.5, y: -0.5, z: -0.5 },
            Pnt3 { x:  0.5, y: -0.5, z: -0.5 },
            Pnt3 { x: -0.5, y: -0.5, z:  0.5 },
            Pnt3 { x:  0.5, y: -0.5, z:  0.5 },
        ];
        CUBE_VERTICES.iter().cloned()
    }

    fn indices(&self) -> <Cube as Model>::IndicesIter {
        static CUBE_INDICES: [u16; 36] = [
            0, 1, 3, 0, 3, 2, // Top
            4, 5, 1, 4, 1, 0, // Front
            5, 7, 3, 5, 3, 1, // Right
            7, 6, 2, 7, 2, 3, // Back
            6, 4, 0, 6, 0, 2, // Left
            7, 5, 4, 7, 4, 6, // Bottom
        ];
        CUBE_INDICES.iter().cloned()
    }
}

pub fn build_city() -> Scene {
    const WIDTH: usize = 10;
    const LENGTH: usize = 10;
    const TILE_SIZE: f32 = 5.0;
    static PLACEMENT_MATRIX: [[u32; WIDTH]; LENGTH] = [
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 1, 1, 1, 0, 0, 1, 1, 1, 1],
        [1, 1, 1, 0, 0, 0, 0, 1, 1, 1],
        [1, 1, 1, 0, 0, 0, 0, 1, 1, 1],
        [1, 1, 1, 1, 0, 0, 1, 1, 1, 1],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
    ];

    let mut scene = Scene::new();

    // Place the floor
    let floor_transformation = {
        let trans = Iso3::new(Vec3::new(
            -(WIDTH as f32) / 2.0 * TILE_SIZE,
            -10.0,
            -(LENGTH as f32) / 2.0 * TILE_SIZE),
            Vec3::zero()).to_homogeneous();
        let scale = scale_mat(10.0 * WIDTH as f32 * TILE_SIZE, 1.0,
            10.0 * LENGTH as f32 * TILE_SIZE).to_homogeneous();
        trans * scale
    };
    scene.add_model(&Cube::new(), floor_transformation);

    // Place all the buildings
    for z in 0..LENGTH {
        for x in 0..WIDTH {
            let dx = (x as f32 - WIDTH as f32 / 2.0) * TILE_SIZE;
            let dz = (z as f32 - LENGTH as f32 / 2.0) * TILE_SIZE;
            if PLACEMENT_MATRIX[x][z] == 1 {
                let scale = scale_mat(TILE_SIZE, TILE_SIZE, TILE_SIZE).to_homogeneous();
                let translation = Iso3::new(Vec3::new(dx, 0.5, dz), Vec3::zero()).to_homogeneous();
                scene.add_model(&Cube::new(), translation * scale);
            }
        }
    }

    scene
}

fn scale_mat(x: f32, y: f32, z: f32) -> Mat3<f32> {
    Mat3::new(x, 0.0, 0.0,
              0.0, y, 0.0,
              0.0, 0.0, z)
}
