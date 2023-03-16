#pragma once
#include "GeoObject.h"
#include "ModelImporter.h"

class Model : public GeoObject
{
public:
    ~Model();

    Model(std::string name, std::string filePath);

    void Update(float dt) override;

private:
    ModelImporter* m_Importer;

};