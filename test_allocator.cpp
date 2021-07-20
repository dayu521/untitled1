#include"doctest/doctest.h"

#include"smallobj.h"

using namespace Loki;

TEST_CASE("fuck2")
{
    FixedAllocator f{};
    f.Initialize(sizeof (double),4096);
    double * d=static_cast<double *>(f.Allocate());
    f.Deallocate(d,nullptr);
}
