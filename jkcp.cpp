#include <float.h>
#include <memory.h>

//#define QOL_IMPROVEMENTS

int jkPlayer_enableOrigAspect = 0; // ?

typedef struct rdVector2
{
    float x;
    float y;
} rdVector2;

typedef struct rdVector3
{
    float x;
    float y;
    float z;
} rdVector3;

typedef struct rdMatrix34
{
    rdVector3 rvec;
    rdVector3 lvec;
    rdVector3 uvec;
    rdVector3 scale;
} rdMatrix34;

void rdVector_Copy3(rdVector3* v1, const rdVector3* v2)
{
    v1->x = v2->x;
    v1->y = v2->y;
    v1->z = v2->z;
}

void rdMatrix_TransformPoint34(rdVector3* vOut, const rdVector3* vIn, const rdMatrix34* camera)
{
    vOut->x = camera->lvec.x * vIn->y + camera->uvec.x * vIn->z + camera->rvec.x * vIn->x + camera->scale.x;
    vOut->y = (camera->lvec.y * vIn->y + camera->uvec.y * vIn->z + camera->rvec.y * vIn->x) + camera->scale.y;
    vOut->z = camera->uvec.z * vIn->z + camera->lvec.z * vIn->y + camera->rvec.z * vIn->x + camera->scale.z;
}

struct rdMeshinfo
{
    int numVertices;
    rdVector3* verticesProjected;
};

struct rdClipFrustum
{
    rdVector3 field_0;
    float farTop;
    float bottom;
    float farLeft;
    float right;
    float nearTop;
    float nearLeft;
};

struct sithSector;

struct sithAdjoin
{
    sithSector* sector;
    sithAdjoin* mirror;
    sithAdjoin* next;
    float dist;

    int renderTick;
    int visible;

    int numVertices;
    int* vertexPosIdx;
    rdVector3 normal;
};

struct sithSector
{
    int renderTick;
    int clipVisited;
    sithAdjoin* adjoins;
};

struct rdCamera
{
    float screen_height_half;
    float screen_width_half;

    rdMatrix34 view_matrix;
    rdClipFrustum* cameraClipFrustum;
    float fov_y;
    float screenAspectRatio;

    sithSector* sector;
    rdVector3 vec3_1;

    void rdCamera_PerspProject(rdVector3* out, rdVector3* v)
    {
        out->x = (fov_y / v->y) * v->x + screen_height_half;
        out->y = screen_width_half - (jkPlayer_enableOrigAspect ? screenAspectRatio : 1.0f) * (fov_y / v->y) * v->z;
        out->z = v->y;
    }

    void projectLst(rdVector3* vertices_out, rdVector3* vertices_in, int num_vertices)
    {
        for (int i = 0; i < num_vertices; i++)
        {
            rdCamera_PerspProject(vertices_out, vertices_in);
            ++vertices_in;
            ++vertices_out;
        }
    }
};

struct sithWorld
{
    int renderTick;
    rdVector3* vertices;
    rdVector3* verticesTransformed;
    sithSector* sectors;
    int numSectors;
    sithAdjoin* adjoins;
    int numAdjoins;
};

sithWorld* g_pWorld = nullptr;
rdCamera* g_pCamera = nullptr;

int sithRender_numSectors = 0;
#define SITH_MAX_VISIBLE_SECTORS 1024
sithSector* sithRender_aSectors[SITH_MAX_VISIBLE_SECTORS];

int sithRender_adjoinSafeguard;

rdVector3 sithRender_aVerticesTmp[32];
rdVector3 sithRender_aVerticesTmp_projected[32];



rdVector3* pSourceVert;
rdVector3 workVerts[32];
rdVector3* pDestVert;
int rdClip_faceStatus;

rdMeshinfo meshinfo_out;
int sithRender_lastRenderTick = 0;

int rdClip_Face3W(rdClipFrustum* frustum, rdVector3* vertices, int numVertices)
{
    rdVector3* v3; // edx
    int v5; // ebp
    rdVector3* v6; // esi
    rdVector3* v7; // ecx
    double v9; // st7
    double v12; // st6
    double v15; // st5
    double v18; // st4
    double v20; // st5
    double v22; // st5
    rdVector3* v23; // ecx
    int v24; // eax
    rdVector3* v25; // esi
    rdVector3* v26; // edi
    rdVector3* v27; // ecx
    rdVector3* v28; // edx
    double v30; // st7
    double v34; // st6
    double v37; // st5
    double v40; // st4
    double v42; // st5
    double v44; // st5
    rdVector3* v45; // ecx
    int v46; // eax
    rdVector3* v47; // esi
    rdVector3* v48; // edi
    rdVector3* v49; // ecx
    rdVector3* v50; // edx
    double v52; // st7
    double v56; // st5
    double v57; // st6
    double v60; // st5
    double v61; // st4
    double v64; // st3
    double v66; // st4
    double v68; // st3
    rdVector3* v69; // ecx
    int v70; // eax
    rdVector3* v71; // esi
    rdVector3* v72; // edi
    rdVector3* v73; // ecx
    rdVector3* v74; // edx
    double v76; // st7
    double v79; // st5
    double v80; // st6
    double v83; // st5
    double v84; // st4
    double v87; // st3
    double v89; // st4
    double v91; // st3
    rdVector3* v92; // ecx
    int v93; // eax
    rdVector3* v94; // esi
    rdVector3* v95; // edi
    rdVector3* v96; // ecx
    rdVector3* v97; // edx
    double v98; // st7
    rdVector3* v100; // eax
    rdVector3* v101; // esi
    int v104; // eax
    rdVector3* v105; // esi
    rdVector3* v106; // edi
    rdVector3* v107; // ecx
    rdVector3* v108; // edx
    double v109; // st7
    rdVector3* v111; // eax
    float v112; // [esp+10h] [ebp-8h]
    float v113; // [esp+10h] [ebp-8h]
    float v114; // [esp+10h] [ebp-8h]
    float v115; // [esp+10h] [ebp-8h]
    int v116; // [esp+14h] [ebp-4h]
    int v117; // [esp+14h] [ebp-4h]
    int v118; // [esp+14h] [ebp-4h]
    int v119; // [esp+14h] [ebp-4h]
    float frustuma; // [esp+1Ch] [ebp+4h]
    float frustumb; // [esp+1Ch] [ebp+4h]
    float frustumc; // [esp+1Ch] [ebp+4h]
    float frustumd; // [esp+1Ch] [ebp+4h]
    float numVerticesa; // [esp+24h] [ebp+Ch]
    float numVerticesi; // [esp+24h] [ebp+Ch]
    float numVerticesb; // [esp+24h] [ebp+Ch]
    float numVerticesc; // [esp+24h] [ebp+Ch]
    float numVerticesj; // [esp+24h] [ebp+Ch]
    float numVerticesd; // [esp+24h] [ebp+Ch]
    float numVerticese; // [esp+24h] [ebp+Ch]
    float numVerticesk; // [esp+24h] [ebp+Ch]
    float numVerticesf; // [esp+24h] [ebp+Ch]
    float numVerticesl; // [esp+24h] [ebp+Ch]
    int numVerticesg; // [esp+24h] [ebp+Ch]
    int numVerticesh; // [esp+24h] [ebp+Ch]

    v3 = vertices;
    pSourceVert = vertices;
    v5 = 0;
    v6 = workVerts;
    rdClip_faceStatus = 0;
    pDestVert = workVerts;
    v7 = &vertices[numVertices - 1];
    for (v116 = numVertices; v116 > 0; v116--)
    {
        numVerticesa = frustum->farLeft * v7->y;
        v9 = frustum->farLeft * v3->y;
        if (numVerticesa <= v7->x || v9 <= v3->x)
        {
            if (v7->x != numVerticesa && v9 != v3->x && (v7->x < (double)numVerticesa || v9 > v3->x))
            {
                frustuma = v3->y - v7->y;
                v112 = v3->x - v7->x;
                v12 = v3->y * v7->x - v7->y * v3->x;
                numVerticesi = frustum->farLeft * frustuma - v112;
                if (numVerticesi != 0.0)
                {
                    v12 = v12 / numVerticesi;
                }
                numVerticesb = frustum->farLeft * v12;
                v15 = frustuma;
                if (v15 < 0.0)
                    v15 = -v15;
                v18 = v112;
                if (v18 < 0.0)
                    v18 = -v18;
                if (v15 <= v18)
                    v20 = (numVerticesb - v7->x) / v112;
                else
                    v20 = (v12 - v7->y) / frustuma;
                v6->x = numVerticesb;
                v6->y = v12;
                ++v5;
                ++v6;
                v22 = (v3->z - v7->z) * v20;
                rdClip_faceStatus |= 0x10;
                v6[-1].z = v22 + v7->z;
            }
            if (v9 <= v3->x)
            {
                v23 = v6;
                ++v5;
                ++v6;
                *v23 = *v3;
            }
        }
        v7 = v3++;
    }
    if (v5 < 3)
        return v5;
    v24 = v5;
    v5 = 0;
    v25 = vertices;
    v26 = workVerts;
    pDestVert = vertices;
    pSourceVert = workVerts;
    v27 = &workVerts[v24 - 1];
    v28 = workVerts;

    for (v117 = v24; v117 > 0; v117--)
    {
        numVerticesc = frustum->right * v27->y;
        v30 = frustum->right * v28->y;
        if (numVerticesc >= v27->x || v30 >= v28->x)
        {
            if (v27->x != numVerticesc && v30 != v28->x && (v27->x > (double)numVerticesc || v30 < v28->x))
            {
                frustumb = v28->y - v27->y;
                v113 = v28->x - v27->x;
                v34 = v28->y * v27->x - v27->y * v28->x;
                numVerticesj = frustum->right * frustumb - v113;
                if (numVerticesj != 0)
                {
                    v34 = v34 / numVerticesj;
                }
                numVerticesd = frustum->right * v34;
                v37 = frustumb;
                if (v37 < 0.0)
                    v37 = -v37;
                v40 = v113;
                if (v40 < 0.0)
                    v40 = -v40;
                if (v37 <= v40)
                    v42 = (numVerticesd - v27->x) / v113;
                else
                    v42 = (v34 - v27->y) / frustumb;
                v25->x = numVerticesd;
                v25->y = v34;
                ++v5;
                ++v25;
                rdClip_faceStatus |= 0x20;
                v44 = (v28->z - v27->z) * v42;
                v25[-1].z = v44 + v27->z;
            }
            if (v30 >= v28->x)
            {
                v45 = v25;
                ++v5;
                ++v25;
                v45->x = v28->x;
                v45->y = v28->y;
                v26 = pSourceVert;
                v45->z = v28->z;
            }
        }
        v27 = v28++;
    }

    if (v5 < 3)
        return v5;

    v46 = v5;
    v47 = v26;
    v5 = 0;
    v48 = pDestVert;
    pDestVert = v47;
    pSourceVert = v48;
    v49 = &v48[v46 - 1];
    v50 = v48;
    v118 = v46;
    do
    {
        numVerticese = frustum->farTop * v49->y;
        v52 = v50->y * frustum->farTop;
        if (numVerticese >= v49->z || v52 >= v50->z)
        {
            if (v49->z != numVerticese && v52 != v50->z && (v49->z > (double)numVerticese || v52 < v50->z))
            {
                frustumc = v50->y - v49->y;
                v114 = v50->z - v49->z;
                v56 = v50->y * v49->z - v50->z * v49->y;
                v57 = v56;
                numVerticesk = frustum->farTop * frustumc - v114;
                if (numVerticesk != 0.0)
                {

                    v57 = v56 / numVerticesk;
                }
                v60 = frustum->farTop * v57;
                v61 = frustumc;
                if (v61 < 0.0)
                    v61 = -v61;
                v64 = v114;
                if (v64 < 0.0)
                    v64 = -v64;
                if (v61 <= v64)
                    v66 = (v60 - v49->z) / v114;
                else
                    v66 = (v57 - v49->y) / frustumc;
                ++v5;
                ++v47;
                v68 = (v50->x - v49->x) * v66 + v49->x;
                rdClip_faceStatus |= 0x4;
                v47[-1].x = v68;
                v47[-1].y = v57;
                v47[-1].z = v60;
            }
            if (v52 >= v50->z)
            {
                v69 = v47;
                ++v5;
                ++v47;
                v69->x = v50->x;
                v69->y = v50->y;
                v48 = pSourceVert;
                v69->z = v50->z;
            }
        }
        v49 = v50++;
        --v118;
    } while (v118);
    if (v5 < 3)
        return v5;
    v70 = v5;
    v71 = v48;
    v5 = 0;
    v72 = pDestVert;
    pDestVert = v71;
    pSourceVert = v72;
    v73 = &v72[v70 - 1];
    v74 = v72;
    v119 = v70;
    do
    {
        numVerticesf = frustum->bottom * v73->y;
        v76 = v74->y * frustum->bottom;
        if (numVerticesf <= v73->z || v76 <= v74->z)
        {
            if (v73->z != numVerticesf && v76 != v74->z && (v73->z < (double)numVerticesf || v76 > v74->z))
            {
                frustumd = v74->y - v73->y;
                v115 = v74->z - v73->z;
                v79 = v74->y * v73->z - v74->z * v73->y;
                v80 = v79;
                numVerticesl = frustum->bottom * frustumd - v115;
                if (numVerticesl != 0.0)
                {
                    v80 = v79 / numVerticesl;
                }
                v83 = frustum->bottom * v80;
                v84 = frustumd;
                if (v84 < 0.0)
                    v84 = -v84;
                v87 = v115;
                if (v87 < 0.0)
                    v87 = -v87;
                if (v84 <= v87)
                    v89 = (v83 - v73->z) / v115;
                else
                    v89 = (v80 - v73->y) / frustumd;
                ++v5;
                ++v71;
                v91 = (v74->x - v73->x) * v89 + v73->x;
                rdClip_faceStatus |= 8;
                v71[-1].x = v91;
                v71[-1].y = v80;
                v71[-1].z = v83;
            }
            if (v76 <= v74->z)
            {
                v92 = v71;
                ++v5;
                ++v71;
                v92->x = v74->x;
                v92->y = v74->y;
                v72 = pSourceVert;
                v92->z = v74->z;
            }
        }
        v73 = v74++;
        --v119;
    } while (v119);
    if (v5 < 3)
        return v5;
    v93 = v5;
    v94 = v72;
    v5 = 0;
    v95 = pDestVert;
    pDestVert = v94;
    pSourceVert = v95;
    v96 = &v95[v93 - 1];
    v97 = v95;
    numVerticesg = v93;
    do
    {
        if (v96->y >= (double)frustum->field_0.y || v97->y >= (double)frustum->field_0.y)
        {
            if (v96->y != frustum->field_0.y && v97->y != frustum->field_0.y && (v96->y < (double)frustum->field_0.y || v97->y < (double)frustum->field_0.y))
            {
                ++v5;
                v98 = (frustum->field_0.y - v96->y) / (v97->y - v96->y);
                v94->x = (v97->x - v96->x) * v98 + v96->x;
                v94->y = frustum->field_0.y;
                v94->z = (v97->z - v96->z) * v98 + v96->z;
                rdClip_faceStatus |= 1;

                ++v94;
            }
            if (v97->y >= (double)frustum->field_0.y)
            {
                v100 = v94;
                ++v5;
                ++v94;
                v100->x = v97->x;
                v100->y = v97->y;
                v95 = pSourceVert;
                v100->z = v97->z;
            }
        }
        v96 = v97++;
        --numVerticesg;
    } while (numVerticesg);
    v101 = pDestVert;
    if (v5 < 3)
    {
        rdClip_faceStatus |= 0x40;
        return v5;
    }
    if (frustum->field_0.x)
    {
        v104 = v5;
        v105 = v95;
        v5 = 0;
        v106 = pDestVert;
        pDestVert = v105;
        pSourceVert = v106;
        v107 = &v106[v104 - 1];
        v108 = v106;
        numVerticesh = v104;
        do
        {
            if (v107->y <= (double)frustum->field_0.z || v108->y <= (double)frustum->field_0.z)
            {
                if (v107->y != frustum->field_0.z
                    && v108->y != frustum->field_0.z
                    && (v107->y > (double)frustum->field_0.z || v108->y > (double)frustum->field_0.z))
                {
                    ++v5;
                    v109 = (frustum->field_0.z - v107->y) / (v108->y - v107->y);
                    v105->y = frustum->field_0.z;
                    ++v105;
                    rdClip_faceStatus |= 0x2;
                    v105[-1].z = (v108->z - v107->z) * v109 + v107->z;
                    v105[-1].x = (v108->x - v107->x) * v109 + v107->x;
                }
                if (v108->y <= (double)frustum->field_0.z)
                {
                    v111 = v105;
                    ++v5;
                    ++v105;
                    *v111 = *v108;
                }
            }
            v107 = v108++;
            --numVerticesh;
        } while (numVerticesh);
        if (v5 < 3)
            return v5;
        v101 = pDestVert;
    }
    if (v101 != vertices)
        memcpy(vertices, pDestVert, sizeof(rdVector3) * v5);
    return v5;
}

void rdPrimit3_ClipFace(rdClipFrustum* clipFrustum, sithAdjoin* pAdjoin, rdMeshinfo* mesh_out)
{
    rdMeshinfo* v8; // ebx
    int v9; // esi
    rdVector3* v10; // edi
    rdVector3* v11; // edx
    int* v12; // ecx
    int v13; // eax
    rdVector3* v14; // ebx
    int idxInfoa; // [esp+38h] [ebp+14h]

    v8 = mesh_out;
    v9 = pAdjoin->numVertices;
    idxInfoa = pAdjoin->numVertices;
    if (idxInfoa)
    {
        v10 = g_pWorld->verticesTransformed;//v7->vertices;
        v11 = mesh_out->verticesProjected;
        v12 = pAdjoin->vertexPosIdx;
        do
        {
            v13 = *v12;
            v14 = v11;
            ++v12;
            ++v11;
            --v9;
            *v14 = v10[v13];
        } while (v9);
        v9 = idxInfoa;
        v8 = mesh_out;
    }
    //if (rdCamera_pCurCamera->projectType == 1)
        v8->numVertices = rdClip_Face3W(clipFrustum, v8->verticesProjected, v9);
    //else
        //v8->numVertices = rdClip_Face3WOrtho(clipFrustum, v8->verticesProjected, v9);
}


int rdCamera_BuildClipFrustum(rdCamera* camera, rdClipFrustum* outClip, signed int height, signed int width, signed int height2, signed int width2)
{
    //jk_printf("%u %u %u %u\n", height, width, height2, width2);

    rdClipFrustum* cameraClip = camera->cameraClipFrustum;

#ifdef QOL_IMPROVEMENTS
    float overdraw = 1.0; // Added: HACK for 1px off on the bottom of the screen
#else
    float overdraw = 0.0;
#endif
    float project_width_half = overdraw + camera->screen_width_half - ((double)width - 0.5);
    float project_height_half = overdraw + camera->screen_height_half - ((double)height - 0.5);

    float project_width_half_2 = -camera->screen_width_half + ((double)width2 - 0.5);
    float project_height_half_2 = -camera->screen_height_half + ((double)height2 - 0.5);

    rdVector_Copy3(&outClip->field_0, &cameraClip->field_0);

    float fov_calc = camera->fov_y;
    float fov_calc_height = camera->fov_y;

#ifdef QOL_IMPROVEMENTS
    if (jkPlayer_enableOrigAspect)
        fov_calc_height = camera->fov_y * camera->screenAspectRatio;
#endif

    // UBSAN fixes
    if (fov_calc_height == 0) {
        fov_calc_height = 0.000001;
    }
    if (fov_calc == 0) {
        fov_calc = 0.000001;
    }

    outClip->farTop = project_width_half / fov_calc_height;
    outClip->farLeft = -project_height_half / fov_calc;
    outClip->bottom = -project_width_half_2 / fov_calc_height;
    outClip->right = project_height_half_2 / fov_calc;
    outClip->nearTop = (project_width_half - -1.0) / fov_calc_height;
    outClip->nearLeft = -(project_height_half - -1.0) / fov_calc;

    return 1;
}


void sithRender_Clip(sithSector* sector, rdClipFrustum* frustumArg, float a3)
{
    sithAdjoin* adjoinIter; // ebx
    rdVector3* v20; // eax
    int v25; // eax
    rdClipFrustum* v31; // ecx
    rdClipFrustum outClip; // [esp+Ch] [ebp-74h] BYREF
    int v45; // [esp+4Ch] [ebp-34h]

    if (sector->renderTick == sithRender_lastRenderTick)
    {
        //sector->clipFrustum = rdCamera_pCurCamera->cameraClipFrustum;
    }
    else
    {
        sector->renderTick = sithRender_lastRenderTick;
        if (sithRender_numSectors >= SITH_MAX_VISIBLE_SECTORS)
            return;

        sithRender_aSectors[sithRender_numSectors++] = sector;

        //frustum = &sithRender_clipFrustums[sithRender_numClipFrustums++];
        //memcpy(frustum, frustumArg, sizeof(rdClipFrustum));
        
        //sector->clipFrustum = frustum;
        
        //sithRender_aSectors2[sithRender_numSectors2++] = sector;
    }


    v45 = sector->clipVisited;
    //sithRender_idxInfo.vertices = g_pWorld->verticesTransformed;
    sector->clipVisited = 1;

    // Added: safeguard
    for (adjoinIter = sector->adjoins; adjoinIter != NULL; adjoinIter = adjoinIter->next)
    {
        if (adjoinIter->sector->clipVisited)
        {
            continue;
        }

        // Added
        if (++sithRender_adjoinSafeguard >= 0x100000)
            break;

        v20 = &g_pWorld->vertices[*adjoinIter->vertexPosIdx];
        float dist = (g_pCamera->vec3_1.y - v20->y) * adjoinIter->normal.y
            + (g_pCamera->vec3_1.z - v20->z) * adjoinIter->normal.z
            + (g_pCamera->vec3_1.x - v20->x) * adjoinIter->normal.x;

        if (dist > 0.0 || (dist == 0.0 && sector == g_pCamera->sector))
        {
            if (adjoinIter->renderTick != sithRender_lastRenderTick)
            {
                for (int i = 0; i < adjoinIter->numVertices; i++)
                {
                    v25 = adjoinIter->vertexPosIdx[i];
                    if (g_pWorld->renderTick != sithRender_lastRenderTick)
                    {
                        rdMatrix_TransformPoint34(&g_pWorld->verticesTransformed[v25], &g_pWorld->vertices[v25], &g_pCamera->view_matrix);
                        g_pWorld->renderTick = sithRender_lastRenderTick;
                    }
                }
                adjoinIter->renderTick = sithRender_lastRenderTick;
            }
            else {
                // Added?
                //continue;
            }

            meshinfo_out.verticesProjected = sithRender_aVerticesTmp;

            rdPrimit3_ClipFace(frustumArg, adjoinIter, &meshinfo_out);


            /*int bAdjoinIsTransparent = (adjoinIter->geometryMode == 0) ||
                                        ((adjoinIter->type & 2) != 0) ||
                                        (adjoinIter->matCelAlpha != 0);*/

#ifdef QOL_IMPROVEMENTS
            // Added: Somehow the clipping changed enough to cause a bug in MoTS Lv12.
            // The ground under the water surface somehow renders.
            // As a mitigation, if a mirror surface is transparent but the top-layer isn't,
            // we will render underneath anyways.
            sithSurface* adjoinMirrorSurface = adjoinIter->mirror->surface;
            rdMaterial* adjoinMirrorMat = adjoinMirrorSurface->surfaceInfo.face.material;
            rdTexinfo* adjoinMirrorTexinfo = NULL;
            if (adjoinMirrorMat)
            {
                int v19 = adjoinMirrorSurface->surfaceInfo.face.wallCel;
                if (v19 == -1)
                    v19 = adjoinMirrorMat->celIdx;
                adjoinMirrorTexinfo = adjoinMirrorMat->texinfos[v19];
            }
            else {
                adjoinMirrorTexinfo = NULL; // Added. TODO: does setting this to NULL cause issues?
            }

            int bMirrorAdjoinIsTransparent = (((!adjoinMirrorSurface->surfaceInfo.face.material ||
                (adjoinMirrorSurface->surfaceInfo.face.geometryMode == 0)) ||
                ((adjoinMirrorSurface->surfaceInfo.face.type & 2))) ||
                (adjoinMirrorTexinfo && (adjoinMirrorTexinfo->header.texture_type & 8) && (adjoinMirrorTexinfo->texture_ptr->alpha_en & 1))
                );

            bAdjoinIsTransparent |= bMirrorAdjoinIsTransparent;
#endif

            if ( (((unsigned int)meshinfo_out.numVertices >= 3u) || (rdClip_faceStatus & 0x40))
                && ((rdClip_faceStatus & 0x41) || (adjoinIter->visible/*(adjoinIter->flags & 1) && bAdjoinIsTransparent*/)) )
            {
                g_pCamera->projectLst(sithRender_aVerticesTmp_projected, sithRender_aVerticesTmp, meshinfo_out.numVertices);

                v31 = frustumArg;

                // no frustum culling
                if ((rdClip_faceStatus & 0x41) != 0)
                {
                    v31 = frustumArg;
                }
                else
                {
                    float minX = FLT_MAX;
                    float minY = FLT_MAX;
                    float maxX = -FLT_MAX;
                    float maxY = -FLT_MAX;
                    for (int i = 0; i < meshinfo_out.numVertices; i++)
                    {
                        float v34 = sithRender_aVerticesTmp_projected[i].x;
                        float v57 = sithRender_aVerticesTmp_projected[i].y;
                        if (v34 < minX)
                            minX = v34;
                        if (v34 > maxX)
                            maxX = v34;

                        if (v57 < minY)
                            minY = v57;
                        if (v57 > maxY)
                            maxY = v57;
                    }

                    // Causes random black lines?
#if 0
                    float v49 = ceilf(maxY);
                    float v48 = ceilf(maxX);
                    float v47 = ceilf(minY);
                    float v46 = ceilf(minX);
#endif

                    // Fixed
                    float v49 = maxY + 1.5;
                    float v48 = maxX + 1.5;
                    float v47 = minY - 2.0;//ceilf(minY);
                    float v46 = minX - 2.0;//ceilf(minX);

                    rdCamera_BuildClipFrustum(g_pCamera, &outClip, (int)(v46 - -0.5), (int)(v47 - -0.5), (int)v48, (int)v49);
                    v31 = &outClip;
                }

                // Added: noclip
                //if (sithPlayer_bNoClippingRend) continue;

                float a3a = adjoinIter->dist + adjoinIter->mirror->dist + a3;
                if(a3a < 0.0)//if (!(sithRender_flag & 4) || a3a < sithRender_f_82F4B0) // wtf is with this float?
                    sithRender_Clip(adjoinIter->sector, v31, a3a);
            }
        }
    }
    sector->clipVisited = v45;
}

extern "C"
{
    void __cdecl CPShutdown()
    {
        if (g_pWorld != nullptr)
        {
            for (int x = 0; x < g_pWorld->numAdjoins; x++)
                delete[] g_pWorld->adjoins[x].vertexPosIdx;
            delete[] g_pWorld->adjoins;

            delete[] g_pWorld->sectors;

            delete[] g_pWorld->verticesTransformed;
            delete[] g_pWorld->vertices;

            delete g_pWorld;
            g_pWorld = nullptr;
        }
    }



    struct SmithAdjoin
    {
        int sectorID;
        int mirrorAdjoinID;
        float dist;

        int visible;

        int numVertices;
        int* vertexPosIdx;
        float nx;
        float ny;
        float nz;
    };


    void __cdecl CPInitialize(int numVertices, float* pVertices, int numSectors, int numAdjoins, SmithAdjoin* pSmithAdjoins)
    {
        CPShutdown();

        g_pWorld = new sithWorld;
        g_pWorld->vertices = new rdVector3[numVertices];
        for (int x = 0; x < numVertices; x++)
        {
            g_pWorld->vertices[x].x = pVertices[x*3 + 0];
            g_pWorld->vertices[x].y = pVertices[x*3 + 1];
            g_pWorld->vertices[x].z = pVertices[x*3 + 2];
        }
        g_pWorld->verticesTransformed = new rdVector3[numVertices];// i think this should be same size.  dont bother initializing?

        g_pWorld->sectors = new sithSector[numSectors];
        g_pWorld->numSectors = numSectors;

        g_pWorld->adjoins = new sithAdjoin[numAdjoins];
        g_pWorld->numAdjoins = numAdjoins;
        for (int x = 0; x < numAdjoins; x++)
        {
            const SmithAdjoin& ia = pSmithAdjoins[x];
            sithAdjoin& oa = g_pWorld->adjoins[x];

            oa.sector = &g_pWorld->sectors[ia.sectorID];
            oa.mirror = &g_pWorld->adjoins[ia.mirrorAdjoinID];
            oa.dist = ia.dist;

            oa.visible = ia.visible;

            if (ia.numVertices <= 0 || ia.numVertices > 200)
            {
                oa = oa;
            }

            oa.numVertices = ia.numVertices;
            oa.vertexPosIdx = new int[ia.numVertices];
            memcpy(oa.vertexPosIdx, ia.vertexPosIdx, ia.numVertices * sizeof(int));

            oa.normal.x = ia.nx;
            oa.normal.y = ia.ny;
            oa.normal.z = ia.nz;
        }

        // now we need to form linked-lists within adjoins that map to sector..
        for (int secID = 0; secID < numSectors; secID++)
        {
            sithSector* sec = &g_pWorld->sectors[secID];

            sec->clipVisited = 0;
            sec->renderTick = 0;

            sithAdjoin* tail = nullptr;
            for (int adjID = 0; adjID < numAdjoins; adjID++)
            {
                sithAdjoin* adj = &g_pWorld->adjoins[adjID];
                if (adj->sector != sec)
                    continue;

                // if first one, set head in sector
                if (sec->adjoins == nullptr)
                    sec->adjoins = adj;

                // link to previous tail (if exist)
                if (tail != nullptr)
                    tail->next = adj;

                // this is new tail
                tail = adj;
                tail->next = nullptr;
            }
        }

    }


    void __cdecl CPSolve(int* pAdjoinVisible)
    {
        sithRender_adjoinSafeguard = 0;

        // reset sector stuff
        for (int x = 0; x < g_pWorld->numSectors; x++)
        {
            sithSector& sec = g_pWorld->sectors[x];
            
            sec.renderTick = 0;
            sec.clipVisited = 0;
        }

        // reset and update adjoin stuff
        for (int x = 0; x < g_pWorld->numAdjoins; x++)
        {
            sithAdjoin& adj = g_pWorld->adjoins[x];

            adj.renderTick = 0;
            adj.visible = pAdjoinVisible[x];
        }

        //sithRender_Clip(g_pCamera->sector, g_pCamera->cameraClipFrustum, 0.0);
    }
}