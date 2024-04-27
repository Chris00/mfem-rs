#[cxx::bridge]
pub mod ffi {
    #[repr(i32)]
    enum BasisType {
        Invalid = -1,
        /// Open type
        GaussLegendre = 0,
        /// Closed type
        GaussLobatto = 1,
        /// Bernstein polynomials
        Positive = 2,
        /// Nodes: x_i = (i+1)/(n+1), i=0,...,n-1
        OpenUniform = 3,
        /// Nodes: x_i = i/(n-1),     i=0,...,n-1
        ClosedUniform = 4,
        /// Nodes: x_i = (i+1/2)/n,   i=0,...,n-1
        OpenHalfUniform = 5,
        /// Serendipity basis (squares / cubes)
        Serendipity = 6,
        /// Closed GaussLegendre
        ClosedGL = 7,
        /// Integrated GLL indicator functions
        IntegratedGLL = 8,
        /// Keep track of maximum types to prevent hard-coding
        NumBasisTypes = 9,
    }

    #[repr(i32)]
    enum OrderingType {
        /// loop first over the nodes (inner loop) then over the vector dimension (outer loop)
        /// symbolically it can be represented as: XXX...,YYY...,ZZZ...
        byNODES,
        /// loop first over the vector dimension (inner loop) then over the nodes (outer loop)
        /// symbolically it can be represented as: XYZ,XYZ,XYZ,...
        byVDIM,
    }

    unsafe extern "C++" {
        // https://github.com/dtolnay/cxx/issues/280

        include!("mfem-sys/include/wrapper.hpp");

        /////////////////////////////
        // FiniteElementCollection //
        /////////////////////////////

        type FiniteElementCollection;

        fn FiniteElementCollection_Name(fec: &FiniteElementCollection) -> *const c_char;

        /////////////////////
        // H1_FECollection //
        /////////////////////

        type H1_FECollection;

        #[cxx_name = "construct_unique"]
        fn H1_FECollection_ctor(
            p: i32,
            dim: i32,
            btype: /*BasisType*/ i32,
        ) -> UniquePtr<H1_FECollection>;

        fn H1_FECollection_as_fec(h1_fec: &H1_FECollection) -> &FiniteElementCollection;

        //////////
        // Mesh //
        //////////

        type Mesh;

        #[cxx_name = "construct_unique"]
        fn Mesh_ctor() -> UniquePtr<Mesh>;

        #[cxx_name = "construct_unique"]
        fn Mesh_ctor_file(
            filename: &CxxString,
            generate_edges: i32,
            refine: i32,
            fix_orientation: bool,
        ) -> UniquePtr<Mesh>;

        fn Mesh_Dimension(mesh: &Mesh) -> i32;
        fn Mesh_GetNE(mesh: &Mesh) -> i32;
        fn Mesh_UniformRefinement(mesh: Pin<&mut Mesh>, ref_algo: i32);
        fn Mesh_GetNodes(mesh: &Mesh) -> Result<&GridFunction>;

        ////////////////////////
        // FiniteElementSpace //
        ////////////////////////

        type OrderingType;

        type FiniteElementSpace<'mesh, 'fec>;

        fn FiniteElementSpace_ctor<'mesh, 'fec>(
            mesh: Pin<&'mesh mut Mesh>,
            fec: &'fec FiniteElementCollection,
            vdim: i32,
            ordering: OrderingType,
        ) -> UniquePtr<FiniteElementSpace<'mesh, 'fec>>;

        //////////////////
        // GridFunction //
        //////////////////

        type GridFunction;

        fn GridFunction_OwnFEC(grid_func: &GridFunction) -> *const FiniteElementCollection;
    }
}
