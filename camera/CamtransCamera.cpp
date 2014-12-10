/**
 * @file   CamtransCamera.cpp
 *
 * This is the perspective camera class you will need to fill in for the Camtrans lab.  See the
 * lab handout for more details.
 */

#include "CamtransCamera.h"
//#include <Settings.h>

CamtransCamera::CamtransCamera()
{
    m_near = 1.0;
    m_far = 30.0;
    m_aspectRatio = 1.0;
    m_heightAngle = 2 * M_PI / 6;
    // not this
    m_widthAngle = glm::atan(glm::tan(m_heightAngle / 2) * m_aspectRatio) * 2;
    m_eye = glm::vec4(2, 2, 2, 0);
    m_look = glm::normalize(glm::vec4(0, 0, 0, 0) - m_eye);
    m_up = glm::vec4(0, 1, 0, 0);

    orientLook(m_eye, m_look, m_up);

}

void CamtransCamera::setAspectRatio(float a)
{
    m_aspectRatio = a;
    m_widthAngle = glm::atan(glm::tan(m_heightAngle / 2) * m_aspectRatio) * 2;
}

glm::mat4x4 CamtransCamera::getProjectionMatrix() const
{
    double cotw = 1 / glm::tan(m_widthAngle / 2);
    double coth = 1 / glm::tan(m_heightAngle / 2);
    glm::mat4x4 smat = glm::transpose(
                glm::mat4x4(cotw/m_far, 0, 0, 0,
                            0, coth/m_far, 0, 0,
                            0, 0, 1/m_far, 0,
                            0, 0, 0, 1));
    return getPerspectiveMatrix() * smat;
}

glm::mat4x4 CamtransCamera::getViewMatrix() const
{
    // Positions the camera
    glm::mat4x4 rmat = glm::transpose(
                glm::mat4x4(m_u.x, m_u.y, m_u.z, 0,
                            m_v.x, m_v.y, m_v.z, 0,
                            m_w.x, m_w.y, m_w.z, 0,
                            0, 0, 0, 1));
    glm::mat4x4 tmat = glm::transpose(
                glm::mat4x4(1, 0, 0, -1 * m_eye.x,
                            0, 1, 0, -1 * m_eye.y,
                            0, 0, 1, -1 * m_eye.z,
                            0, 0, 0, 1));
    return rmat * tmat;
}

glm::mat4x4 CamtransCamera::getScaleMatrix() const
{
    double cotw = 1 / glm::tan(m_widthAngle / 2);
    double coth = 1 / glm::tan(m_heightAngle / 2);
    glm::mat4x4 smat = glm::transpose(
                glm::mat4x4(cotw/m_far, 0, 0, 0,
                            0, coth/m_far, 0, 0,
                            0, 0, 1/m_far, 0,
                            0, 0, 0, 1));

    return smat;
}

glm::mat4x4 CamtransCamera::getPerspectiveMatrix() const
{
    double c = -1 * (m_near / m_far);
    glm::mat4x4 pmat = glm::transpose(
                glm::mat4x4(1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, -1/(c+1), c/(c+1),
                            0, 0, -1, 0));

    return pmat;
}

glm::vec4 CamtransCamera::getPosition() const
{
    return m_eye;
}

glm::vec4 CamtransCamera::getLook() const
{
    return m_look;
}

glm::vec4 CamtransCamera::getUp() const
{
    return m_v;
}

float CamtransCamera::getAspectRatio() const
{
    return m_aspectRatio;
}

float CamtransCamera::getHeightAngle() const
{
    return m_heightAngle * 180 / M_PI;
}

void CamtransCamera::orientLook(const glm::vec4 &eye, const glm::vec4 &look, const glm::vec4 &up)
{
    // @TODO: [CAMTRANS] Fill this in...
    m_look = glm::normalize(look);
    m_up = glm::normalize(up);
    m_eye = eye;

    m_w = glm::normalize(-look);
    m_v = glm::normalize(up - glm::dot(up,m_w)*m_w);
    glm::vec3 u3d = glm::cross(
                glm::vec3(m_v.x, m_v.y, m_v.z),
                glm::vec3(m_w.x, m_w.y, m_w.z));
    m_u = glm::vec4(u3d.x, u3d.y, u3d.z, 0);

}

void CamtransCamera::setHeightAngle(float h)
{
    m_heightAngle = h * M_PI / 180.0;
    m_widthAngle = glm::atan(glm::tan(m_heightAngle / 2) * m_aspectRatio) * 2;
}

void CamtransCamera::translate(const glm::vec4 &v)
{
    glm::mat4x4 tmat = glm::transpose(
                glm::mat4x4(1, 0, 0, v.x,
                            0, 1, 0, v.y,
                            0, 0, 1, v.z,
                            0, 0, 0, 1));

    glm::vec4 newval = tmat * glm::vec4(m_eye.x, m_eye.y, m_eye.z, 1);
    m_eye = newval;

}

void CamtransCamera::rotateW(float degrees)
{
    float rads = degrees * M_PI / 180;
    glm::vec4 u2 = m_v * glm::sin(rads) + m_u * glm::cos(rads);
    glm::vec4 v2 = m_v * glm::cos(rads) - m_u * glm::sin(rads);
    m_u = u2;
    m_v = v2;
//    m_look = glm::normalize(-m_w);
//    m_up = glm::normalize(m_v);
    m_up = glm::normalize(glm::vec4(m_v.x, m_v.y, m_v.z, 0));
    m_look = glm::normalize(glm::vec4(-m_w.x, -m_w.y, -m_w.z, 0));
//    glm::mat4x4 xmat = glm::transpose(
//                glm::mat4x4(
//                    1
//                    ));

//    m_look = one axis, m_up is another?
    // w unchanged
}

void CamtransCamera::rotateU(float degrees)
{
    float rads = degrees * M_PI / 180;
    glm::vec4 v2 = m_v * glm::cos(rads) + m_w * glm::sin(rads);
    glm::vec4 w2 = m_w * glm::cos(rads) - m_v * glm::sin(rads) ;
    m_v = v2;
    m_w = w2;

//    m_up = glm::normalize(m_v);
//    m_look = glm::normalize(-m_w);
    m_up = glm::normalize(glm::vec4(m_v.x, m_v.y, m_v.z, 0));
    m_look = glm::normalize(glm::vec4(-m_w.x, -m_w.y, -m_w.z, 0));
    // u unchanged
}

void CamtransCamera::rotateV(float degrees)
{
    float rads = degrees * M_PI / 180;
    glm::vec4 u2 = m_u * glm::cos(rads) - m_w * glm::sin(rads);
    glm::vec4 w2 = m_u * glm::sin(rads) + m_w * glm::cos(rads);
    m_u = u2;
    m_w = w2;
    m_up = glm::normalize(glm::vec4(m_v.x, m_v.y, m_v.z, 0));
    m_look = glm::normalize(glm::vec4(-m_w.x, -m_w.y, -m_w.z, 0));
    // v unchanged
}

void CamtransCamera::setClip(float nearPlane, float farPlane)
{
    m_near = nearPlane;
    m_far = farPlane;
}

