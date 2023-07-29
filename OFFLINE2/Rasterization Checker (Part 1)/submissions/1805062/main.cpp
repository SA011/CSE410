#include "process.cpp"

void init()
{
    scene >> eye.x >> eye.y >> eye.z;
    scene >> look.x >> look.y >> look.z;
    scene >> up.x >> up.y >> up.z;
    scene >> field_of_view.y;
    scene >> aspect_ratio >> near >> far;


    //view Model
    l = {look.x - eye.x , look.y - eye.y , look.z - eye.z};

    l = normalization(l);
    r = normalization(crossProduct(l, up));
    u = crossProduct(r, l);


    //projectionModel
    field_of_view.x = field_of_view.y * aspect_ratio;
    // t and r from projection model
    tP = near * tan((field_of_view.y * M_PI) / 360);
    rP = near * tan((field_of_view.x * M_PI) / 360);
    diff = far - near;
}

int main()
{
    init();

    viewTransformation();
    projectionTransform();
    modelTransform();

    return 0;
}
