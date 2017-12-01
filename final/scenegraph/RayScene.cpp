#include "RayScene.h"
#include "Settings.h"
#include "CS123SceneData.h"
#include "Camera.h"
#include "BGRA.h"

#include <qcoreapplication.h>

#include <iostream>
#define PI 3.141592653571828
#define MAX_REFLECT 4


RayScene::RayScene(Scene &scene) :
    Scene(scene)
{
    // TODO [INTERSECT]
    // Remember that any pointers or OpenGL objects (e.g. texture IDs) will
    // be deleted when the old scene is deleted (assuming you are managing
    // all your memory properly to prevent memory leaks).  As a result, you
    // may need to re-allocate some things here.

    m_Primitives.clear();
    m_Lights.clear();
    imagemap.clear();

    setGlobal(scene.m_global);
    for (int i = 0; i < scene.m_Primitives.size(); i++){
        CS123ScenePrimitive temp = *(scene.m_Primitives[i].first);
        this->m_Primitives.push_back(std::make_pair(std::make_unique<CS123ScenePrimitive>(temp), scene.m_Primitives[i].second));
        if (imagemap.count(temp.material.textureMap.filename)==0){
        imagemap[temp.material.textureMap.filename] = QImage(QString::fromStdString(temp.material.textureMap.filename));
        }
    }

    for (int i = 0; i < scene.m_Lights.size(); i++){
        this->m_Lights.push_back(std::make_unique<CS123SceneLightData>(*scene.m_Lights[i]));
    }
}

void RayScene::render(Canvas2D* canvas, Camera* camera, int width, int height){
     canvas->resize(width, height);

     BGRA *pix = canvas->data();

     glm::mat4 M_FilmToWorld = glm::inverse(camera->getViewMatrix()) * glm::inverse(camera->getScaleMatrix());
     glm::vec4 p_eye = glm::inverse(camera->getViewMatrix()) * glm::vec4(0,0,0,1);

     for (int i = 0; i < canvas->height(); i++){
         for (int j = 0; j <  canvas->width(); j++){
             //calculate the ray for each pixel
             QCoreApplication::processEvents();
             glm::vec4 Pfilm = glm::vec4(j * 2.0 / width - 1.f, 1.f - i* 2.0 / height, -1.f, 1);
             glm::vec4 farWorld = M_FilmToWorld * Pfilm;
             glm::vec4 d_world = glm::normalize(farWorld - p_eye);


             int index = i*canvas->width()+j;
             glm::vec3 final = renderRecursive(p_eye, d_world, 0);

             float finalcolorR = std::min(std::max(final.x*255.0f+0.5f,0.0f),255.0f);
             float finalcolorG = std::min(std::max(final.y*255.0f+0.5f,0.0f),255.0f);
             float finalcolorB = std::min(std::max(final.z*255.0f+0.5f,0.0f),255.0f);
             int r = finalcolorR;
             int g = finalcolorG;
             int b = finalcolorB;

             pix[index]= BGRA(r, g, b, 0);
         }
     }
     canvas->update();


}

glm::vec3 RayScene::renderRecursive(glm::vec4 p_eye, glm::vec4 d_world, int recLevel){
    if (recLevel >= MAX_REFLECT) return glm::vec3();

    float best_t= std::numeric_limits<float>::infinity();
    int bestIndex = -1;
    glm::vec4 n_local = glm::vec4(0,0,0,0);
    glm::vec4 n_world = glm::vec4(0,0,0,0);
    glm::vec4 intersect_world = glm::vec4(0,0,0,0);
    glm::vec2 uv = glm::vec2(-1,-1);

    bool find = findIntersect(p_eye, d_world, best_t, n_local, bestIndex);
    if (find){
        n_world = glm::vec4(glm::normalize(glm::mat3(glm::transpose(glm::inverse(m_Primitives[bestIndex].second))) * n_local.xyz()), 0);
        intersect_world = d_world * best_t + p_eye + 0.001f*n_world;
        uv = calculateUV(bestIndex, glm::inverse(m_Primitives[bestIndex].second)*p_eye + best_t * glm::inverse(m_Primitives[bestIndex].second)*d_world, n_local);

        if (settings.useReflection && recLevel+1 < MAX_REFLECT){
            glm::vec4 reflect_world = glm::normalize(2.f * n_world * glm::dot(n_world, -d_world) + d_world);

            glm::vec3 temp = calculateColor(bestIndex, intersect_world, n_world, uv, d_world);

            glm::vec3 temp2 = renderRecursive(intersect_world, reflect_world, recLevel+1);

            CS123SceneMaterial curmat = m_Primitives[bestIndex].first->material;

            glm::vec3 refl = glm::vec3(curmat.cReflective.x, curmat.cReflective.y, curmat.cReflective.z);

            return glm::min(glm::vec3(1.f), temp + m_global.ks * refl * temp2);
        }
        else return calculateColor(bestIndex, intersect_world, n_world, uv, d_world);
    }


    return glm::vec3();
}



void RayScene::cancelRender(){

}


glm::vec2 RayScene::calculateUV(int bestIndex, glm::vec4 intersect, glm::vec4 n_local){
    switch (m_Primitives[bestIndex].first->type) {
    case PrimitiveType::PRIMITIVE_CONE:
        return uvCone(intersect, n_local);
        break;
    case PrimitiveType::PRIMITIVE_CUBE:
        return uvCube(intersect, n_local);
        break;
    case PrimitiveType::PRIMITIVE_CYLINDER:
        return uvCylinder(intersect, n_local);
        break;
    case PrimitiveType::PRIMITIVE_SPHERE:
        return uvSphere(intersect, n_local);
        break;
    default:
        return glm::vec2(-1.0f,-1.0f);
        break;
    }

}

glm::vec3 RayScene::calculateColor(int bestIndex, glm::vec4 intersect, glm::vec4 n_world,
                                   glm::vec2 uv, glm::vec4 d_world){
    if(bestIndex != -1){

       CS123SceneMaterial curmat = m_Primitives[bestIndex].first->material;
       //ambient
       float finalcolorR = m_global.ka*curmat.cAmbient.x;
       float finalcolorG = m_global.ka*curmat.cAmbient.y;
       float finalcolorB = m_global.ka*curmat.cAmbient.z;

       //diffuse
       float diffuseR = 0;
       float diffuseG = 0;
       float diffuseB = 0;

       //spec
       float specR = 0;
       float specG = 0;
       float specB = 0;



       for (int l = 0; l < m_Lights.size(); l++){
           CS123SceneLightData curlight = *m_Lights[l];
           float func = 0;

           //attenuation
           float att = 1.0f;
           glm::vec4 refl = glm::vec4();

           glm::vec4 dir =  glm::vec4();

           if (curlight.type == LightType::LIGHT_POINT){
               if(settings.usePointLights == false) continue;
               else{
                   dir = glm::normalize(curlight.pos - intersect);
                   func = glm::dot(n_world, dir);
                   func = std::min(std::max(func,0.0f),1.0f);

                   float dist = glm::distance(curlight.pos, intersect);
                   att = std::min(1.f, 1.f / (curlight.function.x + curlight.function.y * dist + curlight.function.z * dist * dist));

                   refl = 2.0f*func*n_world+dir;
               }
           }
           else if (curlight.type == LightType::LIGHT_DIRECTIONAL){
               if(settings.useDirectionalLights == false) continue;
               else{
                   dir = glm::normalize(-curlight.dir);
                   func = glm::dot(n_world, dir);
                   func = std::min(std::max(func,0.0f),1.0f);

                   refl = 2.0f*func*n_world+glm::normalize(curlight.dir);
               }
           }


           glm::vec3 spec = glm::vec3(curmat.cSpecular.r, curmat.cSpecular.g, curmat.cSpecular.b);
           spec = glm::max(glm::vec3(), spec * std::pow(glm::max(0.f, glm::dot(refl, glm::normalize(d_world))), curmat.shininess));
           spec = spec*m_global.ks;
           specR += spec.x;
           specG += spec.y;
           specB += spec.z;

           glm::vec3 difcolor = m_global.kd*curmat.cDiffuse.xyz();

           if(settings.useTextureMapping){
               glm::vec3 color = textureColor(bestIndex, uv);
               if (color.x != -1){
                   difcolor = (1-curmat.blend)*difcolor + curmat.blend*color;
               }
           }

           if(settings.useShadows){
               float t_temp = std::numeric_limits<float>::infinity();
               glm::vec4 n_world_temp = glm::vec4(0,0,0,0);
               int index_temp = -1;
               bool isIntersect = findIntersect(intersect+0.00001f*n_world, dir, t_temp, n_world_temp, index_temp);
               if(!isIntersect/* || index_temp == bestIndex*/){
                   diffuseR += att*curlight.color.x * difcolor.x*func;
                   diffuseG += att*curlight.color.y * difcolor.y*func;
                   diffuseB += att*curlight.color.z * difcolor.z*func;
               }

           }
           else{
               diffuseR += att*curlight.color.x * difcolor.x*func;
               diffuseG += att*curlight.color.y * difcolor.y*func;
               diffuseB += att*curlight.color.z * difcolor.z*func;
           }

       }



       finalcolorR+=diffuseR;
       finalcolorG+=diffuseG;
       finalcolorB+=diffuseB;

       finalcolorR+=specR;
       finalcolorG+=specG;
       finalcolorB+=specB;

       return glm::vec3(finalcolorR, finalcolorG, finalcolorB);

    }
    else{
        return glm::vec3();
    }

}

bool RayScene::findIntersect(glm::vec4 p_eye, glm::vec4 d_world, float &best_t, glm::vec4 &n, int &bestIndex){
    bool findany = false;
    for (int k = 0; k < m_Primitives.size(); k++){
        glm::vec4 n_local = glm::vec4(0,0,0,0);
        bool find = false;
        switch (m_Primitives[k].first->type) {
        case PrimitiveType::PRIMITIVE_CUBE:
            find = intersectCube(glm::inverse(m_Primitives[k].second)*p_eye, glm::inverse(m_Primitives[k].second)*d_world, best_t, n_local);
            break;
        case PrimitiveType::PRIMITIVE_SPHERE:
            find = intersectSphere(glm::inverse(m_Primitives[k].second)*p_eye, glm::inverse(m_Primitives[k].second)*d_world, best_t, n_local);
            break;
        case PrimitiveType::PRIMITIVE_CONE:
            find =  intersectCone(glm::inverse(m_Primitives[k].second)*p_eye, glm::inverse(m_Primitives[k].second)*d_world, best_t, n_local);
            break;
        case PrimitiveType::PRIMITIVE_CYLINDER:
            find =  intersectCylinder(glm::inverse(m_Primitives[k].second)*p_eye, glm::inverse(m_Primitives[k].second)*d_world, best_t, n_local);
            break;
        default:
            break;
        }

        if (find){
            n = n_local;
            bestIndex = k;
            findany = true;
        }
    }
    return findany;

}

bool RayScene::intersectCube(glm::vec4 p_eye, glm::vec4 d, float &t, glm::vec4 &n){
    float t_orig = t;

    if(d.x!=0){
        //x = 0.5
        float t_temp = (0.5f-p_eye.x)/d.x;
        glm::vec4 intersectpoint = p_eye+d*t_temp;
        if (intersectpoint.y <=0.5 && intersectpoint.y >=-0.5 && intersectpoint.z <=0.5 && intersectpoint.z >=-0.5){
            if (t_temp >=0 && t_temp < t) {
                n = glm::vec4(1,0,0,0);
                t = t_temp;
            }
        }


        //x = -0.5
        t_temp = (-0.5f-p_eye.x)/d.x;
        intersectpoint = p_eye+d*t_temp;
        if (intersectpoint.y <=0.5 && intersectpoint.y >=-0.5 && intersectpoint.z <=0.5 && intersectpoint.z >=-0.5){
            if (t_temp >=0 && t_temp < t) {
                n = glm::vec4(-1,0,0,0);
                t = t_temp;
            }
        }
    }

    if(d.y!=0){
        //y = 0.5
        float t_temp = (0.5f-p_eye.y)/d.y;
        glm::vec4 intersectpoint = p_eye+d*t_temp;
        if (intersectpoint.x <=0.5 && intersectpoint.x >=-0.5 && intersectpoint.z <=0.5 && intersectpoint.z >=-0.5){
            if (t_temp >=0 && t_temp < t) {
                n = glm::vec4(0,1,0,0);
                t = t_temp;
            }
        }

        //y = -0.5
        t_temp = (-0.5f-p_eye.y)/d.y;
        intersectpoint = p_eye+d*t_temp;
        if (intersectpoint.x <=0.5 && intersectpoint.x >=-0.5 && intersectpoint.z <=0.5 && intersectpoint.z >=-0.5){
            if (t_temp >=0 && t_temp < t) {
                n = glm::vec4(0,-1,0,0);
                t = t_temp;
            }
        }
    }

    if(d.z!=0){
        //z = 0.5
        float t_temp = (0.5f-p_eye.z)/d.z;
        glm::vec4 intersectpoint = p_eye+d*t_temp;
        if (intersectpoint.y <=0.5 && intersectpoint.y >=-0.5 && intersectpoint.x <=0.5 && intersectpoint.x >=-0.5){
            if (t_temp >=0 && t_temp < t) {
                n = glm::vec4(0,0,1,0);
                t = t_temp;
            }
        }


        //z = -0.5
        t_temp = (-0.5f-p_eye.z)/d.z;
        intersectpoint = p_eye+d*t_temp;
        if (intersectpoint.y <=0.5 && intersectpoint.y >=-0.5 && intersectpoint.x <=0.5 && intersectpoint.x >=-0.5){
            if (t_temp >=0 && t_temp < t) {
                n = glm::vec4(0,0,-1,0);
                t = t_temp;
            }
        }
    }
    if (t == t_orig) return false;
    else return true;
}

bool RayScene::intersectSphere(glm::vec4 p_eye, glm::vec4 d, float &t, glm::vec4 &n){
    float a = d.x*d.x+d.y*d.y+d.z*d.z;
    float b = 2*(d.x*p_eye.x+d.y*p_eye.y+d.z*p_eye.z);
    float c = p_eye.x*p_eye.x+p_eye.y*p_eye.y+p_eye.z*p_eye.z-0.25f;
    float del = b*b-4*a*c;
    if (del < 0) return false;
    else{
        float t_orig = t;
        float root1 = (-b + glm::sqrt(del))/(2*a);
        float root2 = (-b - glm::sqrt(del))/(2*a);
        if (root1 >=0 && root1 < t){
            t = root1;
            n = glm::normalize(p_eye+t*d - glm::vec4(0,0,0,0));
        }

        if (root2 >=0 && root2 < t){
            t = root2;
            n = glm::normalize(p_eye+t*d - glm::vec4(0,0,0,0));
        }
        if (t == t_orig) return false;
        else return true;
    }

}

bool RayScene::intersectCone(glm::vec4 p_eye, glm::vec4 d, float &t, glm::vec4 &n){

    float a = d.x*d.x+d.z*d.z-0.25f*d.y*d.y;
    float b = 2*(d.x*p_eye.x+d.z*p_eye.z) - 0.5f*d.y*p_eye.y + 0.25*d.y;
    float c = p_eye.x*p_eye.x+p_eye.z*p_eye.z - 0.25f*p_eye.y*p_eye.y  + 0.25*p_eye.y  - 0.0625f;
    float del = b*b-4*a*c;
    float t_orig = t;

    if(del >= 0){
        float root1 = (-b + glm::sqrt(del))/(2*a);
        float root2 = (-b - glm::sqrt(del))/(2*a);
        bool sidenorm = false;
        glm::vec4 intersectpoint = p_eye+d*root1;
        if (root1 >=0 && root1 < t &&  intersectpoint.y >=-0.5f && intersectpoint.y<=0.5f){
            t = root1;
            sidenorm = true;
        }
        intersectpoint = p_eye+d*root2;
        if (root2 >=0 && root2 < t && intersectpoint.y >=-0.5f && intersectpoint.y<=0.5f){
            t = root2;
            sidenorm = true;
        }

        if(sidenorm){
            glm::vec3 inter = glm::vec3((p_eye+t*d).xyz());
            glm::vec3 vert = glm::vec3(0,0.5,0);
            glm::vec3 par = glm::vec3(inter.x,0,inter.z);
            glm::vec3 side = vert - inter;
            glm::vec3 prod = glm::cross(vert, par);
            n = glm::normalize(glm::vec4(glm::cross(prod,side),0));
        }
    }

    float t_temp = (-0.5f-p_eye.y)/d.y;
    glm::vec4 intersectpoint = p_eye+d*t_temp;
    if (intersectpoint.x * intersectpoint.x + intersectpoint.z * intersectpoint.z <=0.25f){
        if (t_temp >=0 && t_temp < t) {
            n = glm::vec4(0,-1,0,0);
            t = t_temp;
        }
    }

    if (t == t_orig) return false;
    else return true;
}

bool RayScene::intersectCylinder(glm::vec4 p_eye, glm::vec4 d, float &t, glm::vec4 &n){
    float t_orig = t;
    glm::vec4 intersectpoint;

    float t_temp = (-0.5f-p_eye.y)/d.y;
    intersectpoint = p_eye+d*t_temp;
    if (intersectpoint.x * intersectpoint.x + intersectpoint.z * intersectpoint.z <=0.25f){
        if (t_temp >=0 && t_temp < t) {
            n = glm::vec4(0,-1,0,0);
            t = t_temp;
        }
    }

    t_temp = (0.5f-p_eye.y)/d.y;
    intersectpoint = p_eye+d*t_temp;
    if (intersectpoint.x * intersectpoint.x + intersectpoint.z * intersectpoint.z <=0.25f){
        if (t_temp >=0 && t_temp < t) {
            n = glm::vec4(0,1,0,0);
            t = t_temp;
        }
    }



    float a = d.x*d.x+d.z*d.z;
    float b = 2*(d.x*p_eye.x+d.z*p_eye.z) ;
    float c = p_eye.x*p_eye.x+p_eye.z*p_eye.z -0.25f;
    float del = b*b-4*a*c;
    if(del >= 0){
        float root1 = (-b + glm::sqrt(del))/(2*a);
        float root2 = (-b - glm::sqrt(del))/(2*a);

        intersectpoint = p_eye+d*root1;
        if (root1 >=0 && root1 < t &&  intersectpoint.y >=-0.5f && intersectpoint.y<=0.5f){
            t = root1;
            intersectpoint.y = 0;
            n = glm::normalize(intersectpoint);
        }
        intersectpoint = p_eye+d*root2;
        if (root2 >=0 && root2 < t && intersectpoint.y >=-0.5f && intersectpoint.y<=0.5f){
            t = root2;
            intersectpoint.y = 0;
            n = glm::normalize(intersectpoint);
        }
    }


    if (t == t_orig) return false;
    else return true;

}

glm::vec2 RayScene::uvCube(glm::vec4 p, glm::vec4 n){
    if (n.x == 1.0f) return glm::vec2(0.5f - p.z, 0.5f - p.y);
    if (n.x == -1.0f) return glm::vec2(0.5f + p.z, 0.5f - p.y);
    if (n.y == 1.0f) return glm::vec2(0.5f + p.x, 0.5f + p.z);
    if (n.y == -1.0f) return glm::vec2(0.5f + p.x, 0.5f - p.z);
    if (n.z == 1.0f) return glm::vec2(p.x+0.5f, -p.y + 0.5f);
    if (n.z == -1.0f) return glm::vec2(0.5f - p.x, -p.y + 0.5f);
    return glm::vec2(-1.f);
}

glm::vec2 RayScene::uvCone(glm::vec4 p, glm::vec4 n){
    if (n.y == -1.0f){
        return glm::vec2(p.x+0.5f,1.0f-(p.z+0.5f));
    }
    else{
        float u = (std::atan2(-p.z, -p.x) + PI) / (2.f * PI);
        return glm::vec2(1.0f-u, 1.0f-(0.5f+p.y));
    }
}


glm::vec2 RayScene::uvSphere(glm::vec4 p, glm::vec4 n){
    float y = std::atan2(glm::sqrt(n.x * n.x + n.z * n.z), n.y) / PI;
    float x = (std::atan2(-n.z, -n.x) + PI) / (2.f * PI);
    return glm::vec2(1.f - x, y);
}

glm::vec2 RayScene::uvCylinder(glm::vec4 p, glm::vec4 n){
    if (n.y == 1.0f) {
        return glm::vec2(p.x + 0.5f, 0.5f + p.z);
    }

    if (n.y == -1.0f) {
        return glm::vec2(0.5f+p.x, 0.5f-p.z);
    }

    float u = (std::atan2(-p.z, -p.x) + PI) / (2.f * PI);
    return glm::vec2(1.0f - u, 0.5f - p.y);
}

glm::vec3 RayScene::textureColor(int bestIndex, glm::vec2 uv){

    if(uv.x==-1.0f) return glm::vec3(0.0f);
    CS123SceneMaterial curmat = m_Primitives[bestIndex].first->material;
    if (!curmat.textureMap.isUsed) return glm::vec3(-1,-1,-1);
    QImage* image = &imagemap[curmat.textureMap.filename];

    glm::vec2 st = glm::vec2(uv.x * curmat.textureMap.repeatU * image->width(), uv.y * curmat.textureMap.repeatV * image->height());

    int rrow = ((int) st.y) % image->height();
    int ccol = ((int) st.x) % image->width();

    QColor clr = QColor::fromRgb(image->pixel(ccol, rrow));
    return glm::vec3(clr.red()/255.0f, clr.green()/255.0f, clr.blue()/255.0f);
}

RayScene::~RayScene(){
}
