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

