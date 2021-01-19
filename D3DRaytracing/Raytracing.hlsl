#define INFINITY 3.402823466e+38F

struct Ray
{
    float3 mOrigin;
    float3 mDirection;
    
    float3 at(float t) { return mOrigin + t * mDirection; }
    
};

struct Camera
{
    float3 mOrigin;
    float3 mLowerLeftCorner;
    float3 mHorizontal;
    float3 mVertical;
    
    Ray GetRay(float u, float v)
    {
        Ray r =
        {
            mOrigin, mLowerLeftCorner + u * mHorizontal + v * mVertical - mOrigin
        };
        
        return r;
    }
};

struct HitRecord
{
    float3 mPoint;
    float3 mNormal;
    float mT;
};

float rand(in float2 uv)
{
    float2 noise = (frac(sin(dot(uv, float2(12.9898, 78.233) * 2.0)) * 43758.5453));
    return abs(noise.x + noise.y) * 0.5;
}




float LengthSquared(float3 t)
{
    return t.x * t.x + t.y * t.y + t.z * t.z;
}

float Length(float3 t)
{
    return sqrt(LengthSquared(t));
}

struct Sphere
{
    float3 mCenter;
    float mRadius;
    
    HitRecord record;
    
    bool hit(Ray r, float tMin, float tMax, out HitRecord rec)
    {
        float3 oc = -(r.mOrigin - mCenter);
        float a = length(r.mDirection) * length(r.mDirection);
        float bHalf = dot(oc, r.mDirection);
        float c = length(oc) * length(oc) - mRadius * mRadius;
        
        float discirminant = bHalf * bHalf - a * c;
        if (discirminant < 0)
            return false;
        
        float sqrtD = sqrt(discirminant);
        
        float root = (-bHalf - sqrtD) / (float) a;
        
        if(root < tMin || tMax < root)
        {
            root = (-bHalf + sqrtD) / (float) a;
            if (root < tMin || tMax < root)
                return false;
        }
        
        rec.mT = root;
        rec.mPoint = r.at(rec.mT);
        rec.mNormal = (rec.mPoint - mCenter) / (float) (mRadius);

        return true;
    }
};

struct HittableList
{

    
    bool hit(in Sphere records[16], Ray r, float tMin, float tMax, inout HitRecord rec)
    {
        HitRecord tempRec = (HitRecord)0;
        
        bool hitAnything = false;
        float closestSoFar = tMax;
        
        for (int i = 0; i < 3; i++)
        {
            if (records[i].hit(r, tMin, closestSoFar, tempRec) == true)
            {
                hitAnything = true;
                closestSoFar = tempRec.mT;
                rec = tempRec;

            }
            


        }
        
        return hitAnything;
    }
    
}; groupshared HittableList hittableList;


RWTexture2D<float4> outTexture : register(u0);



static const float3 origin = float3(0.0, 0.0f,-0.4f);
static const float viewportHeight = 2.0;
static const float aspectRatio = 8.0f / 6.0f;
static const float viewportWidth = aspectRatio * viewportHeight;
static const float3 horizontal = float3(viewportWidth, 0, 0);
static const float3 vertical = float3(0, viewportHeight, 0);
static const float3 lowerLeftCorner = origin - horizontal / 2.0f - vertical / 2.0f - float3(0, 0, 1.0f);
static const int maxDepth = 100;
static const int maxBounce = 2;
static const int samplesPerPixel = 32;

static const Camera mainCam = { origin, lowerLeftCorner, horizontal, vertical };

groupshared HittableList world = (HittableList) 0;
groupshared Sphere gSpheres[16];


float randRange(float min, float max, float2 uv)
{
    return min + (max - min) * rand(uv);
}
float3 randVector(float min, float max, float2 uv)
{
    return float3(randRange(min, max, uv), randRange(min, max, uv), randRange(min, max, uv));

}
float3 randomInUnitSphere(float2 uv)
{
    float3 p = 0.0f;

    while (true)
    {
        p = randVector(-1, 1, uv);
        if ((length(p) * length(p)) >= 1)
            continue;
        return p;
    }
    
    return p;

}

float4 rayColor(inout Ray r,  HittableList world, float2 uv)
{
    float4 white = float4(1, 1, 1, 1);
    HitRecord rec = (HitRecord) 0;
    float4 finalColor = 0;
    int d = 0;
    
    if (world.hit(gSpheres, r, 0.001f, INFINITY, rec) == true)
    {
        while (true)
        {
            if (d >= maxDepth)
                break;
        
            if (world.hit(gSpheres, r, 0.001f, INFINITY, rec) == true)
            {
                float3 target = rec.mPoint + rec.mNormal + randomInUnitSphere(uv);
              
                r.mOrigin = rec.mPoint;
                r.mDirection = (target - rec.mPoint);
                return finalColor *= 0.5f;
              //  finalColor *= float4(1, 1, 1,1);

            }

            d++;
        }
    }
    else
    {
        float3 unitDirection = normalize(r.mDirection);
        float t = 0.5f * (unitDirection.y + 1.0f);
        finalColor = float4((1.0f - t) * float3(1, 1, 1) + t * float3(0.5f, 0.7f, 1.0f), 1.0f);

    }

    return finalColor;
    //if (world.hit(gSpheres, r, 0.0f, INFINITY, rec) == true)
    //{
    //    float3 target = rec.mPoint + rec.mNormal + randomInUnitSphere(uv);
    //    rNew.mOrigin = rec.mPoint;
    //    rNew.mDirection = target - rec.mPoint;
        
    //    for (int i = 0; i < depth; i++)
    //    {

    //        if (world.hit(gSpheres, rNew, 0.0f, INFINITY, rec) == true)
    //        {
    //            target = rec.mPoint + rec.mNormal + randomInUnitSphere(uv);

    //            rNew.mOrigin = rec.mPoint;
    //            rNew.mDirection = target - rec.mPoint;
                
    //            white *= 0.1f;
    //            finalColor += white;
    //         //   finalColor *= finalColor;
    //        }
    //    }
            

    //}

}

float3 WriteColor(float3 pixelColor, int samplesPerPixel)
{
    float r = pixelColor.x;
    float g = pixelColor.y;
    float b = pixelColor.z;
    
    float scale = 1.0f / samplesPerPixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);
    
    pixelColor.x = clamp(r, 0.0f, 0.999);
    pixelColor.y = clamp(g, 0.0f, 0.999);
    pixelColor.z = clamp(b, 0.0f, 0.999);
    
    return pixelColor;
}

[numthreads(16,12, 1)]
void rayTraceCS( uint3 DTid : SV_DispatchThreadID, uint3 Gid : SV_GroupID, uint Gi : SV_GroupIndex, uint3 GTid : SV_GroupThreadID )
{
    if(Gi == 0)
    {
        Sphere s1 = (Sphere) 0, s2 = (Sphere) 0;
        s1.mCenter = float3(0, 0.0, 0);
        s1.mRadius = 0.25f;
       
        s2.mCenter = float3(5.0f, -101, 0.0f);
        s2.mRadius = 100.0f;
        
        gSpheres[1] = s1;
        gSpheres[0] = s2;
        for (int i = 2; i < 16; i++)
        {
            gSpheres[i] = (Sphere) 0;
        }


    }
    GroupMemoryBarrierWithGroupSync();

    float2 uv = float2(DTid.x, DTid.y) / (float2(800, 600));
    float2 adjustedUV = float2(DTid.x + rand(uv), DTid.y + rand(uv)) / float2(800, 600);
    float3 pixelColor = float3(0, 0, 0);
    int d = 0;
    Ray r = mainCam.GetRay(adjustedUV.x, adjustedUV.y);
    
    for (int i = 0; i < samplesPerPixel; i++)
    {
        pixelColor += rayColor(r, world, adjustedUV).xyz;
        
    }
    
    pixelColor = WriteColor(pixelColor, samplesPerPixel);

    outTexture[DTid.xy] = float4(pixelColor, 1.0f);

}