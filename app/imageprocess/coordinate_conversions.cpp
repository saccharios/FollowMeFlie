#include "camera.h"
#include "math/types.h"
cv::Size Camera::_resolution = cv::Size();
void Camera::SetResolution(cv::Size resolution)
{
    _resolution = resolution;
}

MidPoint Camera::ConvertCameraToMidPointCoord(cv::KeyPoint keyPoint)
{
    // Camera coordinate system is, (0,0) is in the top left corner
    // positive x to the right (in pixel)
    // positive y to the bottowm (in pixel)

    // MidPoint cooridnate system is, (0,0) in the middle
    // positive x to the left (in pixel)
    // positive y to the top (in pixel)

    MidPoint midPoint;
    midPoint.pt.x = -keyPoint.pt.x + _resolution.width / 2;
    midPoint.pt.y = -keyPoint.pt.y + _resolution.height/ 2;
    midPoint.size = keyPoint.size;
    return midPoint;
}
std::vector<MidPoint> Camera::ConvertCameraToMidPointCoord(std::vector<cv::KeyPoint> const & keyPoints)
{
    std::vector<MidPoint> midPoints;
    midPoints.reserve(keyPoints.size());
    for(auto const & keyPoint : keyPoints)
    {
        midPoints.push_back(ConvertCameraToMidPointCoord(keyPoint));
    }
    return midPoints;
}

cv::KeyPoint  Camera::ConvertMidPointToCameraCoord(MidPoint midPoint )
{
    cv::KeyPoint keyPoint;
    keyPoint.pt.x = -midPoint.pt.x + Camera::_resolution.width/2;
    keyPoint.pt.y = -midPoint.pt.y + _resolution.height/2;
    keyPoint.size = midPoint.size;
    return keyPoint;
}
std::vector<cv::KeyPoint> Camera::ConvertMidPointToCameraCoord(std::vector<MidPoint> const & midPoints)
{
    std::vector<cv::KeyPoint> keyPoints;
    keyPoints.reserve(midPoints.size());
    for(auto const & midPoint : midPoints)
    {
        keyPoints.push_back(ConvertMidPointToCameraCoord(midPoint));
    }
    return keyPoints;
}

Point3f Camera::ConvertMidPointToCrazyFlieCoord(MidPoint midPoint)
{
    // Crazyflie coordinate system is: (0,0,0) is at the crazyflie
    // 1)The x-direction is positive in the front direction (antenna) (in meter)
    // 2)The y-direction is positive left (in meter)
    // 3)The z-direction is positive top (in meter)

    Point3f point;
    // crazyflie x component depens only on the size,
    // x_cflie = a/y + b
    // See Distance_Measurements_Coord.ods for the data behind these values.

    point.x = (_a/midPoint.size + _b)/100.0f; // factor 100 to convert to meter
    if(point.x < 0)
    {
        point.x = 0.1f;
    }
    // For y and z component, use simple height similarity
    // Ähnlichkeitszeits, gegeben die focal length.
    point.y = midPoint.pt.x * point.x / _focalLength;
    point.z = midPoint.pt.y * point.x / _focalLength;
    return point;
}

MidPoint Camera::ConvertCrazyFlieCoordToMidPoint(Point3f crazyFliePoint)
{
    // Inverse operation to ConvertMidPointToCrazyFlieCoord()
    MidPoint midPoint;
    midPoint.size = _a/(crazyFliePoint.x*100.0f - _b);
    if(midPoint.size < 0)
    {
        midPoint.size = 0.1f;
    }
    midPoint.pt.x = crazyFliePoint.y / crazyFliePoint.x * _focalLength;
    midPoint.pt.y = crazyFliePoint.z / crazyFliePoint.x * _focalLength;

    return midPoint;
}

Point3f Camera::ConvertCameraToCrazyFlieCoord(cv::KeyPoint keyPoint)
{
    return ConvertMidPointToCrazyFlieCoord(ConvertCameraToMidPointCoord(keyPoint));

}

std::vector<Point3f> Camera::ConvertCameraToCrazyFlieCoord(std::vector<cv::KeyPoint> const & keyPoints)
{
    std::vector<Point3f> crazyFliePoints;
    crazyFliePoints.reserve(keyPoints.size());
    for(auto const & keyPoint : keyPoints)
    {
        crazyFliePoints.push_back(ConvertCameraToCrazyFlieCoord(keyPoint));
    }
    return crazyFliePoints;
}
cv::KeyPoint Camera::ConvertCrazyFlieCoordToCameraCoord(Point3f crazyFliePoint)
{
    return ConvertMidPointToCameraCoord(ConvertCrazyFlieCoordToMidPoint(crazyFliePoint));
}
