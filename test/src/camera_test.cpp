#include "camera_test.h"

TEST_F(CameraTest, CrazyFlieToMidPtConversions)
{
    MidPoint midPoint;
    midPoint.pt.x = 10;
    midPoint.pt.y = 10;
    midPoint.size = 33;

    MidPoint converted = Camera::ConvertCrazyFlieCoordToMidPoint(Camera::ConvertMidPointToCrazyFlieCoord(midPoint));
    EXPECT_FLOAT_EQ(converted.pt.x,midPoint.pt.x);
    EXPECT_FLOAT_EQ(converted.pt.y,midPoint.pt.y);
    EXPECT_FLOAT_EQ(converted.size,midPoint.size);

}
