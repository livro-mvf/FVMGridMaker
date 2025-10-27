// tests/Grid/Common/ut_Tags1D.cpp
#include <gtest/gtest.h>
#include <string_view>

#include <FVMGridMaker/Grid/Common/Tags1D.hpp>

using FVMGridMaker::grid::CenteringTag;
using FVMGridMaker::grid::DistributionTag;
using FVMGridMaker::grid::to_string;

TEST(Tags1D, ToStringCentering) {
    EXPECT_EQ(to_string(CenteringTag::FaceCentered), std::string_view{"FaceCentered"});
    EXPECT_EQ(to_string(CenteringTag::CellCentered), std::string_view{"CellCentered"});

    // Índice fora do range deve retornar "Unknown"
    auto bad = static_cast<CenteringTag>(255);
    EXPECT_EQ(to_string(bad), std::string_view{"Unknown"});
}

TEST(Tags1D, ToStringDistribution) {
    EXPECT_EQ(to_string(DistributionTag::Uniform1D), std::string_view{"Uniform1D"});
    EXPECT_EQ(to_string(DistributionTag::Random1D),  std::string_view{"Random1D"});

    auto bad = static_cast<DistributionTag>(255);
    EXPECT_EQ(to_string(bad), std::string_view{"Unknown"});
}
