#include "rust/cxx.h"

///////////////////////////////////////////
// Error Handler for Returning Result<T> //
///////////////////////////////////////////

class mfem_exception : public std::exception {
    std::string msg_;

public:
    explicit mfem_exception(char const* msg) : msg_(msg) {}
    auto what() const noexcept -> char const* override {
        return msg_.c_str();
    }
};

namespace rust {
namespace behavior {

template <typename Try, typename Fail>
static void trycatch(Try &&func, Fail &&fail) noexcept try {
  func();
} catch (const std::exception &e) {
  fail(e.what());
}

} // namespace behavior
} // namespace rust

#include "mfem.hpp"

// Generic template constructor
template <typename T, typename... Args>
auto construct_unique(Args... args) -> std::unique_ptr<T> {
    return std::make_unique<T>(args...);
}

using namespace mfem;

//////////////
// ArrayInt //
//////////////

using ArrayInt = Array<int>;

auto ArrayInt_SetAll(ArrayInt& array, int value) -> void {
    array = value;
}

/////////////////////
// H1_FECollection //
/////////////////////

auto H1_FECollection_as_fec(H1_FECollection const& h1_fec) -> FiniteElementCollection const& {
    return h1_fec;
}

//////////
// Mesh //
//////////

auto Mesh_GetNodes(Mesh const& mesh) -> GridFunction const& {
    auto ptr = mesh.GetNodes();
    if (!ptr) {
        throw mfem_exception("Mesh::GetNodes() == nullptr");
    }
    return *ptr;
}

auto Mesh_bdr_attributes(Mesh const& mesh) -> ArrayInt const& {
    return mesh.bdr_attributes;
}

////////////////////////
// FiniteElementSpace //
////////////////////////

using OrderingType = Ordering::Type;

auto FiniteElementSpace_ctor(Mesh const& mesh, FiniteElementCollection const& fec, int vdim, OrderingType ordering) -> std::unique_ptr<FiniteElementSpace> {
    // HACK(mkovaxx): This might come back to bite me...
    auto& mut_mesh = const_cast<Mesh&>(mesh);
    return std::make_unique<FiniteElementSpace>(&mut_mesh, &fec, vdim, ordering);
}

auto FiniteElementSpace_GetEssentialTrueDofs(
    FiniteElementSpace const& fespace,
    ArrayInt const& bdr_attr_is_ess,
    ArrayInt& ess_tdof_list,
    int component
) -> void {
    // HACK(mkovaxx): This might come back to bite me...
    auto& mut_fespace = const_cast<FiniteElementSpace&>(fespace);
    mut_fespace.GetEssentialTrueDofs(bdr_attr_is_ess, ess_tdof_list, component);
}

//////////////////
// GridFunction //
//////////////////

auto GridFunction_OwnFEC(GridFunction const& grid_func) -> FiniteElementCollection const& {
    auto ptr = const_cast<GridFunction&>(grid_func).OwnFEC();
    if (!ptr) {
        throw mfem_exception("GridFunction::OwnFEC() == nullptr");
    }
    return *ptr;
}

////////////////
// LinearForm //
////////////////

auto LinearForm_ctor_fes(FiniteElementSpace const& fespace) -> std::unique_ptr<LinearForm> {
    // HACK(mkovaxx): This might come back to bite me...
    auto& mut_fespace = const_cast<FiniteElementSpace&>(fespace);
    return std::make_unique<LinearForm>(&mut_fespace);
}

/////////////////////////
// ConstantCoefficient //
/////////////////////////

auto ConstantCoefficient_as_coeff(ConstantCoefficient const& coeff) -> Coefficient const& {
    return coeff;
}

////////////////////////
// DomainLFIntegrator //
////////////////////////

auto DomainLFIntegrator_ctor_ab(Coefficient const& coeff, int a, int b) -> std::unique_ptr<DomainLFIntegrator> {
    // HACK(mkovaxx): This might come back to bite me...
    auto& mut_coeff = const_cast<Coefficient&>(coeff);
    return std::make_unique<DomainLFIntegrator>(mut_coeff, a, b);
}
