//
// Created by captain on 2021/7/10.
//

#include "login_scene.h"
#include <rttr/registration>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/ext.hpp>
#include "utils/application.h"
#include "utils/screen.h"
#include "renderer/camera.h"
#include "renderer/mesh_filter.h"
#include "renderer/material.h"
#include "renderer/mesh_renderer.h"
#include "renderer/texture2d.h"
#include "component/game_object.h"
#include "component/transform.h"
#include "control/key_code.h"
#include "audio/audio_source.h"


using namespace rttr;
RTTR_REGISTRATION
{
    registration::class_<LoginScene>("LoginScene")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

LoginScene::LoginScene() :Component(){}

void LoginScene::Awake() {
    //创建相机1 GameObject
    auto go_camera_1=new GameObject("main_camera");
    //挂上 Transform 组件
    transform_camera_1_=dynamic_cast<Transform*>(go_camera_1->AddComponent("Transform"));
    transform_camera_1_->set_position(glm::vec3(0, 0, 10));
    //挂上 Camera 组件
    camera_1_=dynamic_cast<Camera*>(go_camera_1->AddComponent("Camera"));
    camera_1_->set_depth(0);

    last_frame_mouse_position_=Input::mousePosition();

    CreateSounds();
    CreatePlayer();
}

AudioSource * LoginScene::CreateAudioSource(string mesh_path, string material_path, string audio_path, string name,
                                            glm::vec3 pos) {
    //创建模型 GameObject
    GameObject* go=new GameObject(name);
    if(mesh_path.empty()==false){
        //挂上 Transform 组件
        auto transform =dynamic_cast<Transform*>(go->AddComponent("Transform"));
        transform->set_position(pos);
        //挂上 MeshFilter 组件
        auto mesh_filter=dynamic_cast<MeshFilter*>(go->AddComponent("MeshFilter"));
        mesh_filter->LoadMesh(mesh_path);
        //挂上 MeshRenderer 组件
        auto mesh_renderer=dynamic_cast<MeshRenderer*>(go->AddComponent("MeshRenderer"));
        auto material =new Material();//设置材质
        material->Parse(material_path);
        mesh_renderer->SetMaterial(material);
    }
    //挂上AudioSource
    auto audio_source=dynamic_cast<AudioSource*>(go->AddComponent("AudioSource"));
    audio_source->set_audio_clip(AudioClip::LoadFromFile(audio_path));
    return audio_source;
}

void LoginScene::CreateSounds() {
    // 战斗背景音乐
    audio_source_bgm_= CreateAudioSource("",
                                         "", "audio/war_bgm.wav", "audio_source_bgm", glm::vec3(0,0,0));
    // 刀攻击音效
    audio_source_knife_= CreateAudioSource("model/cube_audio_source.mesh",
                                           "material/audio_source_3d_cube_knife.mat", "audio/knife_attack.wav",
                                           "audio_source_knife", glm::vec3(2,0,0));
    // 魔法攻击音效
    audio_source_magic_= CreateAudioSource("model/cube_audio_source.mesh",
                                           "material/audio_source_3d_cube_magic.mat", "audio/magic_attack.wav",
                                           "audio_source_magic", glm::vec3(-2,0,0));
}

void LoginScene::Update() {
    camera_1_->SetView(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    camera_1_->SetProjection(60.f, Screen::aspect_ratio(), 1.f, 1000.f);

    //旋转相机
    if(Input::GetKeyDown(KEY_CODE_LEFT_ALT) && Input::GetMouseButtonDown(MOUSE_BUTTON_LEFT)){
        float degrees= Input::mousePosition().x_ - last_frame_mouse_position_.x_;

        glm::mat4 old_mat4=glm::mat4(1.0f);

        glm::mat4 rotate_mat4=glm::rotate(old_mat4,glm::radians(degrees),glm::vec3(0.0f,1.0f,0.0f));//以相机所在坐标系位置，计算用于旋转的矩阵，这里是零点，所以直接用方阵。
        glm::vec4 old_pos=glm::vec4(transform_camera_1_->position(),1.0f);
        glm::vec4 new_pos=rotate_mat4*old_pos;//旋转矩阵 * 原来的坐标 = 相机以零点做旋转。
        std::cout<<glm::to_string(new_pos)<<std::endl;

        transform_camera_1_->set_position(glm::vec3(new_pos));
    }
    last_frame_mouse_position_=Input::mousePosition();

    //鼠标滚轮控制相机远近
    transform_camera_1_->set_position(transform_camera_1_->position() *(10 - Input::mouse_scroll())/10.f);

    //按 1 2 3 播放/暂停 3个音效
    if(Input::GetKeyUp(KEY_CODE_1)){
        PlayPauseSound(audio_source_bgm_);
    }else if(Input::GetKeyUp(KEY_CODE_2)){
        PlayPauseSound(audio_source_knife_);
    }else if(Input::GetKeyUp(KEY_CODE_3)){
        PlayPauseSound(audio_source_magic_);
    }
}

void LoginScene::PlayPauseSound(AudioSource* audio_source) {
    if(audio_source->Paused()){
        audio_source->Play();
    }else{
        audio_source->Pause();
    }
}

void LoginScene::CreatePlayer() {
    GameObject* go=new GameObject("Player");
    transform_player_ =dynamic_cast<Transform*>(go->AddComponent("Transform"));
    auto mesh_filter=dynamic_cast<MeshFilter*>(go->AddComponent("MeshFilter"));
    mesh_filter->LoadMesh("model/cube_audio_source.mesh");
    auto mesh_renderer=dynamic_cast<MeshRenderer*>(go->AddComponent("MeshRenderer"));
    auto material =new Material();//设置材质
    material->Parse("material/audio_source_3d_cube_player.mat");
    mesh_renderer->SetMaterial(material);
}

