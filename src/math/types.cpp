#include "types.h"

Blob GetLargestBlob(std::vector<Blob> const & blobs)
{
    Blob largestBlob;
    if(blobs.size() == 1)
    {
        largestBlob =  blobs.at(0);
    }
    else if(blobs.size() > 1)
    {
        for(auto const & blob : blobs)
        {
            if(blob.size > largestBlob.size)
            {
                largestBlob = blob;
            }
        }
    }
    return largestBlob;
}

void PrintBlobs(std::vector<Blob> const & blobs)
{
    int i = 0;
    for(auto const & blob : blobs)
    {
        textLogger << "Blob " << i
                      << " x: " << blob.point.x
                         << " y: " << blob.point.y
                            << " z: " << blob.point.z << "\n";
        ++i;
    }
}
