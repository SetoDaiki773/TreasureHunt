#include "Mesh.h"
#include "HeapOffset.h"
#include "Common/FileUtil.h"
#include "HeapOffset.h"
#include "Device.h"
#include "Common/VectorOperation.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <d3dx12.h>
#include <iostream>
#include "Manager/TextureManager.h"

std::unique_ptr<TextureManager> MyMesh::mTextureManager;

//----------------------------------------------------
//   マテリアルのロード
//----------------------------------------------------
HRESULT LoadMaterial(TextureManager& textureManager,MyMaterial& dst, const struct aiMaterial* src, const wchar_t* modelFilePath);


MyMaterial::MyMaterial()
    :hasTexture(false)
    , ambient(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f))
    , diffuse(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f))
    , specular(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f))
    ,texture(std::make_shared<Texture>())
{

}


MyMesh::MyMesh()
    :mNumTriangles(0)
    , mMaterial()
    , mIndexBuffer(nullptr)
    , mVertexBuffer(nullptr)
    , mVertexBufferView()
    , mIndexBufferView()
{
    static bool isFirst = true;
    if (isFirst) {
        mTextureManager = std::make_unique<TextureManager>();
        isFirst = false;
    }
}

MyMesh::~MyMesh() {

}


//--------------------------------------------
//   頂点バッファビューの取得
//--------------------------------------------
const D3D12_VERTEX_BUFFER_VIEW* MyMesh::GetVertexBufferView()const {
    return &mVertexBufferView;
}


//---------------------------------------------
//   インデックスバッファビューの取得
//---------------------------------------------
const D3D12_INDEX_BUFFER_VIEW* MyMesh::GetIndexBufferView()const {
    return &mIndexBufferView;
}

//--------------------------------------------
//   マテリアルの取得
//--------------------------------------------
MyMaterial MyMesh::GetMaterial()const {
    return mMaterial;
}

//---------------------------------------------
//    ポリゴン数の取得
//---------------------------------------------
DWORD MyMesh::GetNumTriangles()const {
    return mNumTriangles;
}

//----------------------------------------------------
//    シェダーリソースビューのヒープオフセットを取得
//----------------------------------------------------
unsigned MyMesh::GetSrvHeapOffset()const {
    //return mSrvHeapOffset->GetHeapOffset();
    return mMaterial.texture->GetHeapOffset();
}

//-------------------------------------------
// テクスチャを変更する
//-------------------------------------------
void MyMesh::ChangeTexture(const std::wstring& textureName) {
    mMaterial.hasTexture = true;
    mMaterial.texture.reset();
    mMaterial.texture = mTextureManager->GetTexture(textureName);
}


//----------------------------------------------
//   四角形のメッシュを作る
//----------------------------------------------
void MyMesh::CreateSquareMesh(const std::wstring& textureName) {
    //バーテックス　データ作成
    MyVertex triangleVertices[] =
    {
        { { -1.0f, 1.0f , 0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f} },
        { { -1.0f, -1.0f, 0.0f}, {0.0f,0.0f,0.0f}, {0.0f,1.0f} },
        { {1.0f, 1.0f , 0.0f  },  {0.0f,0.0f,0.0f} ,{1.0f,0.0f} },
        { { 1.0f, -1.0f , 0.0f}, {0.0f,0.0f,0.0f} ,{1.0f,1.0f} }
    };
    ID3D12Device* device = Device::GetDevice();

    //バーテックスバッファ作成
    const UINT vertexBufferSize = sizeof(triangleVertices);
    D3D12_HEAP_PROPERTIES vertexHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC vertexResDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
    device->CreateCommittedResource(
        &vertexHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &vertexResDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&mVertexBuffer));

    UINT8* pVertexDataBegin;
    CD3DX12_RANGE readRange(0, 0);
    mVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
    mVertexBuffer->Unmap(0, nullptr);

    //バーテックスバッファビュー作成
    mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
    mVertexBufferView.StrideInBytes = sizeof(MyVertex);
    mVertexBufferView.SizeInBytes = vertexBufferSize;

    //インデックスデータ作成
    DWORD indices[] =
    {
        2, 3, 1,
        0, 2, 1
    };

    //インデックスバッファ作成
    int indexBufferSize = sizeof(indices);
    D3D12_HEAP_PROPERTIES indexHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC indexResDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);
    device->CreateCommittedResource(
        &indexHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &indexResDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&mIndexBuffer));

    UINT8* pIndexDataBegin;
    mIndexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pIndexDataBegin));
    memcpy(pIndexDataBegin, indices, sizeof(indices));
    mIndexBuffer->Unmap(0, nullptr);

    //インデックスバッファビュー作成
    mIndexBufferView.BufferLocation = mIndexBuffer->GetGPUVirtualAddress();
    mIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
    mIndexBufferView.SizeInBytes = indexBufferSize;
    mNumTriangles = 2;
    mMaterial.hasTexture = true;
    mMaterial.texture = mTextureManager->GetTexture(textureName);
}

void MyMesh::CreateCubeMesh(const std::wstring& textureName) {

    // 頂点を頑張って手書き
    DirectX::XMFLOAT3 pos0{ -1.0f, -1.0f, -1.0f };
    DirectX::XMFLOAT3 pos1{ -1.0f, -1.0f, 1.0f  };
    DirectX::XMFLOAT3 pos2{ 1.0f , -1.0f , -1.0f};
    DirectX::XMFLOAT3 pos3{ 1.0f , -1.0f , 1.0f };
    DirectX::XMFLOAT3 pos4{ -1.0f, 1.0f  , -1.0f};
    DirectX::XMFLOAT3 pos5{ -1.0f, 1.0f  , 1.0f };
    DirectX::XMFLOAT3 pos6{ 1.0f , 1.0f  , -1.0f};
    DirectX::XMFLOAT3 pos7{ 1.0f , 1.0f  , 1.0f };
    // uvもがんばって手書き
    DirectX::XMFLOAT2 uv1{ 0.0f,0.0f };
    DirectX::XMFLOAT2 uv2{ 0.0f,1.0f };
    DirectX::XMFLOAT2 uv3{ 1.0f,0.0f };
    DirectX::XMFLOAT2 uv4{ 1.0f,1.0f };

    // 法線はとりあえず0で・・・
    DirectX::XMFLOAT3 norm{ 0.0f,0.0f,0.0f };

    //バーテックス　データ作成
    MyVertex triangleVertices[] =
    {
        // 下面
        { pos0, norm, uv1 },
        { pos1, norm, uv2 },
        { pos2, norm, uv3 },
        { pos3, norm, uv4 },
        // 上面
        { pos4, norm, uv1 },
        { pos5, norm, uv2 },
        { pos6, norm, uv3 },
        { pos7, norm, uv4 },
        // 右面
        {pos2, norm, uv4},//8
        {pos3, norm, uv3},//9
        {pos6, norm, uv2},//10
        {pos7, norm, uv1},//11
        // 左面
        {pos0, norm, uv4},//12
        {pos1, norm, uv2},//13
        {pos4, norm, uv3},//14
        {pos5, norm, uv1},//15
        // 前面
        {pos1, norm, uv2},//16
        {pos3, norm, uv4},//17
        {pos5, norm, uv1},//18
        {pos7, norm, uv3},//19
        // 後面
        {pos0, norm, uv2},//20
        {pos2, norm, uv4},//21
        {pos4, norm, uv1},//22
        {pos6, norm, uv3}//23
    };
    ID3D12Device* device = Device::GetDevice();
    //バーテックスバッファ作成
    const UINT vertexBufferSize = sizeof(triangleVertices);
    D3D12_HEAP_PROPERTIES vertexHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC vertexResDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
    device->CreateCommittedResource(
        &vertexHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &vertexResDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&mVertexBuffer));

    UINT8* pVertexDataBegin;
    CD3DX12_RANGE readRange(0, 0);
    mVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
    mVertexBuffer->Unmap(0, nullptr);

    //バーテックスバッファビュー作成
    mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
    mVertexBufferView.StrideInBytes = sizeof(MyVertex);
    mVertexBufferView.SizeInBytes = vertexBufferSize;

    //インデックスデータ作成
    DWORD indices[] =
    {
        14,13,15,
        12,13,14,
        11, 8,10,
         9, 8,11,
         1, 2, 3,
         0, 2, 1,
         4, 7, 6,
         5, 7, 4,
        23,20,22,
        21,20,23,
        18,17,19,
        16,17,18,
    };
    //インデックスバッファ作成
    int indexBufferSize = sizeof(indices);
    D3D12_HEAP_PROPERTIES indexHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC indexResDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);
    device->CreateCommittedResource(
        &indexHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &indexResDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&mIndexBuffer));

    UINT8* pIndexDataBegin;
    mIndexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pIndexDataBegin));
    memcpy(pIndexDataBegin, indices, sizeof(indices));
    mIndexBuffer->Unmap(0, nullptr);

    //インデックスバッファビュー作成
    mIndexBufferView.BufferLocation = mIndexBuffer->GetGPUVirtualAddress();
    mIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
    mIndexBufferView.SizeInBytes = indexBufferSize;
    mNumTriangles = 12;
    mMaterial.hasTexture = true;
    mMaterial.texture = mTextureManager->GetTexture(textureName);
   
}


void LoadMeshesFromFile(ID3D12Device* device,const wchar_t* filename,std::vector<MyMesh>& meshes)
{
    if (filename == nullptr)
    {
        assert(!"ファイル名がNULL");
    }
   
    // auto& meshes = settings.meshes;
    //auto inverseU = settings.inverseU;
    //auto inverseV = settings.inverseV;

    const std::string path = ToUTF8(filename);


    Assimp::Importer importer;
    int flag = 0;
    // これでファイルをどうやってロードするかが決まる
    flag |= aiProcess_Triangulate;   // 三角形化
    flag |= aiProcess_PreTransformVertices;
    flag |= aiProcess_CalcTangentSpace;
    flag |= aiProcess_GenSmoothNormals; // 球形をできるだけ滑らかにしてくれるやつ
    flag |= aiProcess_GenUVCoords;
    flag |= aiProcess_RemoveRedundantMaterials;
    flag |= aiProcess_OptimizeMeshes;

    const aiScene* scene = importer.ReadFile(path, flag); // sceneにmodel情報がすべて格納されている。

    if (scene == nullptr)
    {
        // もし読み込みエラーがでたら表示する
        MessageBoxA(0,importer.GetErrorString(),"",MB_OK);
        assert(!"読み込み失敗");
    }
    
    // 読み込んだデータを自分で定義したMesh構造体に変換する
    meshes.clear();
    meshes.resize(scene->mNumMeshes);
    // マテリアルの読み込み
    std::vector<MyMaterial> materials;
    materials.resize(scene->mNumMaterials);
    for (size_t i = 0; i < scene->mNumMaterials; i++) {
        const aiMaterial* material = scene->mMaterials[i];
        if (FAILED(LoadMaterial(*meshes[i].mTextureManager, materials[i], material, filename))) {  // material(読み込まれたデータ)をmaterials[i]に書き込む
            assert(!"マテリアルの読み込みに失敗");
        }
    }

    for (size_t i = 0; i < scene->mNumMeshes; i++)
    {
        const aiMesh* mesh = scene->mMeshes[i];
        if (FAILED(LoadMesh(device, meshes[i], mesh))) {  // mesh(読み込まれたデータ)をmeshes[i]に書き込む
            assert(!"Meshのロードに失敗");
        }
        meshes[i].mMaterial = materials[mesh->mMaterialIndex];  // メッシュに対応したマテリアルを割り当てる
        if (!meshes[i].mMaterial.hasTexture) {
            MyMaterial& material = meshes[i].mMaterial;
            material.texture = meshes[i].mTextureManager->GetTexture(L"Asset/Black.png");
        }
    }

    scene = nullptr;

}

//----------------------------------------
//  meshをロードする
//----------------------------------------
HRESULT LoadMesh(ID3D12Device* device,MyMesh& dst, const aiMesh* src)
{

    aiVector3D zero3D(0.0f, 0.0f, 0.0f);
    aiColor4D zeroColor(0.0f, 0.0f, 0.0f, 0.0f);

    std::vector<MyVertex> vertices;
    vertices.resize(src->mNumVertices);

    // 頂点情報のロード。頂点数の数だけループさせて読み込む
    for (auto i = 0u; i < src->mNumVertices; ++i)
    {
        auto position = &(src->mVertices[i]);   // 頂点位置
        auto normal = &(src->mNormals[i]);      // 頂点法線
        auto uv = (src->HasTextureCoords(0)) ? &(src->mTextureCoords[0][i]) : &zero3D;  // 頂点uv
        //auto tangent = (src->HasTangentsAndBitangents()) ? &(src->mTangents[i]) : &zero3D;
        //auto color = (src->HasVertexColors(0)) ? &(src->mColors[0][i]) : &zeroColor;


        // 自分の作った頂点構造体にロードした頂点情報を入れる
        MyVertex vertex = {};
        vertex.pos = DirectX::XMFLOAT3(position->x, position->y, position->z);
        vertex.normal = DirectX::XMFLOAT3(normal->x, normal->y, normal->z);
        vertex.uv = DirectX::XMFLOAT2(uv->x, uv->y);
        //vertex.Tangent = DirectX::XMFLOAT3(tangent->x, tangent->y, tangent->z);
        //vertex.Color = DirectX::XMFLOAT4(color->r, color->g, color->b, color->a);

        vertices[i] = vertex; // 入れ物に入れる
    }

    dst.mNumTriangles = src->mNumFaces;
    //バーテックスバッファー作成
    const UINT vertexbufferSize = sizeof(MyVertex) * src->mNumVertices;
    D3D12_HEAP_PROPERTIES vHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC vResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexbufferSize);
    device->CreateCommittedResource(
        &vHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &vResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(dst.mVertexBuffer.GetAddressOf()));
    //バーテックスバッファに頂点データを詰め込む
    UINT8* pVertexDataBegin = nullptr;
    CD3DX12_RANGE readRange(0, 0);
    dst.mVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, vertices.data(), vertexbufferSize);
    dst.mVertexBuffer->Unmap(0, nullptr);
    //バーテックスバッファビューを初期化
    dst.mVertexBufferView.BufferLocation = dst.mVertexBuffer->GetGPUVirtualAddress();
    dst.mVertexBufferView.StrideInBytes = sizeof(MyVertex);
    dst.mVertexBufferView.SizeInBytes = vertexbufferSize;

    std::vector<int> indices;
    indices.resize(src->mNumFaces * 3);
    // インデックスの読み込み。トライアングル数だけ繰り返す
    for (auto i = 0u; i < src->mNumFaces; ++i)
    {
        const aiFace& face = src->mFaces[i];
        assert(face.mNumIndices == 3);
        indices[i * 3 + 0] = face.mIndices[0];
        indices[i * 3 + 1] = face.mIndices[1];
        indices[i * 3 + 2] = face.mIndices[2];
    }

    //インデックスバッファーを作成
    const UINT indexbufferSize = static_cast<UINT>(sizeof(int)) * static_cast<UINT>(indices.size());
    D3D12_HEAP_PROPERTIES iHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC iResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(indexbufferSize);
    device->CreateCommittedResource(
        &iHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &iResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(dst.mIndexBuffer.GetAddressOf()));
    //インデックスバッファにインデックスデータを詰め込む
    UINT8* pIndexDataBegin;
    CD3DX12_RANGE readRange2(0, 0);
    dst.mIndexBuffer->Map(0, &readRange2, reinterpret_cast<void**>(&pIndexDataBegin));
    memcpy(pIndexDataBegin, indices.data(), indexbufferSize);
    dst.mIndexBuffer->Unmap(0, nullptr);
    //インデックスバッファビューを初期化
    dst.mIndexBufferView.BufferLocation = dst.mIndexBuffer->GetGPUVirtualAddress();
    dst.mIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
    dst.mIndexBufferView.SizeInBytes = indexbufferSize;

    return S_OK;
}


//-----------------------------------------------
//   マテリアルをロードする
//-----------------------------------------------
HRESULT LoadMaterial(TextureManager& textureManager,MyMaterial& dst, const aiMaterial* src,const wchar_t* modelFileName)
{
    aiColor3D color(0.0f, 0.0f, 0.0f);
    // 拡散反射光
    if (src->Get(AI_MATKEY_COLOR_DIFFUSE,color)== AI_SUCCESS) {
        dst.diffuse.x = color.r;
        dst.diffuse.y = color.g;
        dst.diffuse.z = color.b;
        dst.diffuse.w = 1.0f;
    }
    // ファイルに拡散反射光情報が無いなら適当な数値を入れる
    else {
        dst.diffuse.x = 0.f;
        dst.diffuse.y = 0.f;
        dst.diffuse.z = 0.f;
        dst.diffuse.w = 1.0f;
    }
    // 環境光
    if (src->Get(AI_MATKEY_COLOR_AMBIENT,color) == AI_SUCCESS) {
        dst.ambient.x = color.r;
        dst.ambient.y = color.g;
        dst.ambient.z = color.b;
        dst.ambient.w = 1.0f;
    }
    // ファイルに環境光の情報が無いなら適当な数値を入れる
    else {
        dst.ambient.x = 0.2f;
        dst.ambient.y = 0.2f;
        dst.ambient.z = 0.2f;
        dst.ambient.w = 1.0f;
    }
    // 鏡面反射光
    if (src->Get(AI_MATKEY_COLOR_SPECULAR,color) == AI_SUCCESS) {
        dst.specular.x = color.r;
        dst.specular.y = color.g;
        dst.specular.z = color.b;
        dst.specular.w = 1.0f;
    }
    // ファイルに鏡面反射光の情報が無いなら適当な数値を入れる
    else {
        dst.specular.x = 0.0f;
        dst.specular.y = 0.0f;
        dst.specular.z = 0.0f;
        dst.specular.w = 1.0f;
    }
    // テクスチャ
    aiString path;
    if (src->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS)
    {
        std::cout << path.C_Str() << "\n";
        // テクスチャパスは相対パスで入っているので、ファイルの場所とくっつける
        std::wstring dir = GetDirectoryPathW(modelFileName);
        std::string file = std::string(path.C_Str());
        file = RemoveDirectoryPathA(file);
        std::wstring wFile = dir + ToWideString(file);
        dst.texture = textureManager.GetTexture(wFile);
        dst.hasTexture = true;
    }
    else
    {
        //dst.textureName.clear();
    }
    return S_OK;
}




