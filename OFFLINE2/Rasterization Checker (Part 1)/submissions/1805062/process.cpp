#include "Util.h"

// -------------------------------------------------
ifstream scene("scene.txt");
ofstream stage1("stage1.txt");
ofstream stage2("stage2.txt");
ofstream stage3("stage3.txt");
// -------------------------------------------------

//Global Variable
// view model
point eye, look, up, field_of_view;
point l, r, u;
// project model
double aspect_ratio, near, far , tP , rP , diff;
//matrix for view, projection and model transform
Array2D viewMatrix           = generate_identity_matrix();
Array2D projectionMatrix     = generate_identity_matrix();
Array2D modelTransformMatrix = generate_identity_matrix();

//model transform stack
stack<Array2D> modelTransformStack;
stack<stack<Array2D>> states;

// -------------------------------------------------

//view Transformation - stage 2
void viewTransformation()
{
    Array2D transformMatrix = generate_identity_matrix(); 

    transformMatrix.arr_matrix[0][3] = - eye.x;
    transformMatrix.arr_matrix[1][3] = - eye.y;
    transformMatrix.arr_matrix[2][3] = - eye.z;

    //Apply the following rotation R such that the l aligns with the -Z axis, r with X axis, and u with Y axis.
    Array2D rotationMatrix = generate_identity_matrix();

    rotationMatrix.arr_matrix[0][0] = r.x;
    rotationMatrix.arr_matrix[0][1] = r.y;
    rotationMatrix.arr_matrix[0][2] = r.z;

    rotationMatrix.arr_matrix[1][0] = u.x;
    rotationMatrix.arr_matrix[1][1] = u.y;
    rotationMatrix.arr_matrix[1][2] = u.z;

    rotationMatrix.arr_matrix[2][0] = -l.x;
    rotationMatrix.arr_matrix[2][1] = -l.y;
    rotationMatrix.arr_matrix[2][2] = -l.z; 

    // V = RT
    viewMatrix = matrix_multiplication(rotationMatrix, transformMatrix);
}

// -------------------------------------------------

//projection Transformation - stage 3
void projectionTransform()
{
    projectionMatrix.arr_matrix[0][0] = near / rP;
    projectionMatrix.arr_matrix[1][1] = near / tP;
    projectionMatrix.arr_matrix[2][2] = -(far + near) / diff;
    projectionMatrix.arr_matrix[2][3] = -(2 * far * near) / diff;
    projectionMatrix.arr_matrix[3][2] = -1;
    projectionMatrix.arr_matrix[3][3] = 0;
}

// -------------------------------------------------

//model Transformation - stage1
void modelTransform()
{
    modelTransformStack.push(modelTransformMatrix);
    string sceneLines;
    int Triangle_point_Number = 3;

    while (scene >> sceneLines)
    {
        if (sceneLines == "triangle")
            {

                for(int i = 0; i < Triangle_point_Number ; i++)
                {
                    point x;
                    scene >> x.x >> x.y >> x.z;

                    point transpoint = Transformation(x, modelTransformStack.top());
                    point viewPoint  = Transformation(transpoint, viewMatrix);
                    point projectionPoint = Transformation(viewPoint, projectionMatrix);

                    stage1 << fixed << setprecision(7);
                    stage2 << fixed << setprecision(7);
                    stage3 << fixed << setprecision(7);
                   
                    stage1 << transpoint.x      << " " << transpoint.y      << " " << transpoint.z      << endl;
                    stage2 << viewPoint.x       << " " << viewPoint.y       << " " << viewPoint.z       << endl;
                    stage3 << projectionPoint.x << " " << projectionPoint.y << " " << projectionPoint.z << endl;
                }
                
                stage1 << endl;
                stage2 << endl;
                stage3 << endl;
        }

        else if (sceneLines == "push")
        {
            stack<Array2D>top = modelTransformStack;
            states.push(top);
        }
            
        else if (sceneLines == "pop")
        {
            modelTransformStack = states.top();
            states.pop();
        }

        else if (sceneLines == "rotate")
        {
            double angle;
            point point;
            scene >> angle >> point.x >> point.y >> point.z;
            double radian_angle = angle * radian * -1;
            Array2D rotationMatrix  = Rotation(point,radian_angle);
            Array2D top             = matrix_multiplication(modelTransformStack.top(), rotationMatrix);

            modelTransformStack.push(top);
        }

        else if (sceneLines == "scale")
        {
            point scale;
            scene >> scale.x >> scale.y >> scale.z;

            Array2D scaleMatrix             = generate_identity_matrix();
            scaleMatrix.arr_matrix[0][0]    = scale.x;
            scaleMatrix.arr_matrix[1][1]    = scale.y;
            scaleMatrix.arr_matrix[2][2]    = scale.z;

            Array2D top                     = matrix_multiplication(modelTransformStack.top(), scaleMatrix);
            modelTransformStack.push(top);
        }

        else if (sceneLines == "translate")
        {
            point translate;
            scene >> translate.x >> translate.y >> translate.z;

            Array2D translateMatrix             = generate_identity_matrix();
            translateMatrix.arr_matrix[0][3]    = translate.x;
            translateMatrix.arr_matrix[1][3]    = translate.y;
            translateMatrix.arr_matrix[2][3]    = translate.z;

            Array2D top                         = matrix_multiplication(modelTransformStack.top(), translateMatrix);
            modelTransformStack.push(top);
        }

        else if (sceneLines == "end")
            break;
    }
    stage1.close();
    stage2.close();
    stage3.close();   
}
