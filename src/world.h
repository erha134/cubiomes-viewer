#ifndef WORLD_H
#define WORLD_H

#include "settings.h"
#include "search.h"

#include <QRunnable>
#include <QImage>
#include <QPainter>
#include <QAtomicPointer>
#include <QIcon>
#include <QMutex>

#include "cubiomes/quadbase.h"


enum {
    D_NONE = -1,
    // generics
    D_GRID,
    D_SLIME,
    // structures
    D_DESERT,
    D_JUNGLE,
    D_IGLOO,
    D_HUT,
    D_VILLAGE,
    D_MANSION,
    D_MONUMENT,
    D_RUINS,
    D_SHIPWRECK,
    D_TREASURE,
    D_MINESHAFT,
    D_OUTPOST,
    D_ANCIENTCITY,
    D_PORTAL,
    D_PORTALN,
    D_FORTESS,
    D_BASTION,
    D_ENDCITY,
    D_GATEWAY,
    // non-recurring structures
    D_SPAWN,
    D_STRONGHOLD,
    STRUCT_NUM
};

inline const char *mapopt2str(int opt)
{
    switch (opt)
    {
    case D_GRID:        return "网格";
    case D_SLIME:       return "史莱姆";
    case D_DESERT:      return "沙漠";
    case D_JUNGLE:      return "丛林";
    case D_IGLOO:       return "冰屋";
    case D_HUT:         return "沼泽小屋";
    case D_VILLAGE:     return "村庄";
    case D_MANSION:     return "林地府邸";
    case D_MONUMENT:    return "海底神殿";
    case D_RUINS:       return "海底遗迹";
    case D_SHIPWRECK:   return "沉船";
    case D_TREASURE:    return "宝藏";
    case D_MINESHAFT:   return "废弃矿井";
    case D_OUTPOST:     return "前哨站";
    case D_ANCIENTCITY: return "远古城市";
    case D_PORTAL:      return "废弃传送门";
    case D_PORTALN:     return "废弃传送门(下界)";
    case D_SPAWN:       return "出生点";
    case D_STRONGHOLD:  return "要塞";
    case D_FORTESS:     return "下界要塞";
    case D_BASTION:     return "堡垒遗迹";
    case D_ENDCITY:     return "末地城";
    case D_GATEWAY:     return "末地折越门";
    default:            return "";
    }
}

inline int str2mapopt(const char *s)
{
    if (!strcmp(s, "网格"))         return D_GRID;
    if (!strcmp(s, "史莱姆"))        return D_SLIME;
    if (!strcmp(s, "沙漠"))       return D_DESERT;
    if (!strcmp(s, "丛林"))       return D_JUNGLE;
    if (!strcmp(s, "冰屋"))        return D_IGLOO;
    if (!strcmp(s, "沼泽小屋"))          return D_HUT;
    if (!strcmp(s, "村庄"))      return D_VILLAGE;
    if (!strcmp(s, "林地府邸"))      return D_MANSION;
    if (!strcmp(s, "海底神殿"))     return D_MONUMENT;
    if (!strcmp(s, "海底遗迹"))        return D_RUINS;
    if (!strcmp(s, "沉船"))    return D_SHIPWRECK;
    if (!strcmp(s, "宝藏"))     return D_TREASURE;
    if (!strcmp(s, "废弃矿井"))    return D_MINESHAFT;
    if (!strcmp(s, "前哨站"))      return D_OUTPOST;
    if (!strcmp(s, "远古城市"))         return D_ANCIENTCITY;
    if (!strcmp(s, "废弃传送门"))        return D_PORTAL;
    if (!strcmp(s, "废弃传送门(下界)"))    return D_PORTALN;
    if (!strcmp(s, "出生点"))             return D_SPAWN;
    if (!strcmp(s, "要塞"))               return D_STRONGHOLD;
    if (!strcmp(s, "下界要塞"))           return D_FORTESS;
    if (!strcmp(s, "堡垒遗迹"))         return D_BASTION;
    if (!strcmp(s, "末地城"))          return D_ENDCITY;
    if (!strcmp(s, "末地折越门"))        return D_GATEWAY;
    return D_NONE;
}

inline int mapopt2stype(int opt)
{
    switch (opt)
    {
    case D_DESERT:      return Desert_Pyramid;
    case D_JUNGLE:      return Jungle_Pyramid;
    case D_IGLOO:       return Igloo;
    case D_HUT:         return Swamp_Hut;
    case D_VILLAGE:     return Village;
    case D_MANSION:     return Mansion;
    case D_MONUMENT:    return Monument;
    case D_RUINS:       return Ocean_Ruin;
    case D_SHIPWRECK:   return Shipwreck;
    case D_TREASURE:    return Treasure;
    case D_MINESHAFT:   return Mineshaft;
    case D_OUTPOST:     return Outpost;
    case D_ANCIENTCITY: return Ancient_City;
    case D_PORTAL:      return Ruined_Portal;
    case D_PORTALN:     return Ruined_Portal_N;
    case D_FORTESS:     return Fortress;
    case D_BASTION:     return Bastion;
    case D_ENDCITY:     return End_City;
    case D_GATEWAY:     return End_Gateway;
    default:
        return -1;
    }
}

void loadStructVis(std::map<int, double>& structvis);
void saveStructVis(std::map<int, double>& structvis);

struct Level;

struct VarPos
{
    VarPos(Pos p, int type) : p(p),type(type),v(),pieces() {}
    Pos p;
    int type;
    StructureVariant v;
    std::vector<Piece> pieces;

    QStringList detail() const;
};

const QPixmap& getMapIcon(int opt, VarPos *variation = 0);
QIcon getBiomeIcon(int id, bool warn = false);

void getStructs(std::vector<VarPos> *out, const StructureConfig sconf,
        WorldInfo wi, int dim, int x0, int z0, int x1, int z1, bool nogen = false);

enum {
    HV_GRAYSCALE        = 0,
    HV_SHADING          = 1,
    HV_CONTOURS         = 2,
    HV_CONTOURS_SHADING = 3,
};
void applyHeightShading(unsigned char *rgb, Range r,
        const Generator *g, const SurfaceNoise *sn, int stepbits, int mode,
        bool bicubic, const std::atomic_bool *abort);

struct Scheduled : public QRunnable
{
    Scheduled() : prio(),stopped(),next(),done(),isdel() {}
    // externally managed (read/write in controller thread only)
    float prio;
    int stopped; // not done, and also not in processing queue
    Scheduled *next;
    std::atomic_bool done; // indicates that no further processing will occur
    std::atomic_bool *isdel;
};

struct Quad : public Scheduled
{
    Quad(const Level* l, int64_t i, int64_t j);
    ~Quad();

    void run();

    WorldInfo wi;
    int dim;
    LayerOpt lopt;
    const Generator *g;
    const SurfaceNoise *sn;
    int hd;
    int scale;
    int ti, tj;
    int blocks;
    int pixs;
    int sopt;

    int *biomes;
    uchar *rgb;

    // img and spos act as an atomic gate (with NULL or non-NULL indicating available results)
    QAtomicPointer<QImage> img;
    QAtomicPointer<std::vector<VarPos>> spos;
};

struct QWorld;
struct Level
{
    Level();
    ~Level();

    void init4map(QWorld *world, int pix, int layerscale);
    void init4struct(QWorld *world, int dim, int blocks, double vis, int sopt);

    void resizeLevel(std::vector<Quad*>& cache, int64_t x, int64_t z, int64_t w, int64_t h);
    void update(std::vector<Quad*>& cache, qreal bx0, qreal bz0, qreal bx1, qreal bz1);
    void setInactive(std::vector<Quad*>& cache);

    QWorld *world;
    std::vector<Quad*> cells;
    Generator g;
    SurfaceNoise sn;
    Layer *entry;
    LayerOpt lopt;
    WorldInfo wi;
    int dim;
    int tx, tz, tw, th;
    int hd;
    int scale;
    int blocks;
    int pixs;
    int sopt;
    double vis;
    std::atomic_bool *isdel;
};

struct PosElement
{
    PosElement(Pos p_) : next(), p(p_) {}
    ~PosElement() { delete next; next = nullptr; }
    QAtomicPointer<PosElement> next;
    Pos p;
};

class MapWorker : public QThread
{
    Q_OBJECT
public:
    MapWorker() : QThread(), world() {}
    virtual ~MapWorker() {}
    virtual void run() override;
signals:
    void quadDone();
public:
    QWorld *world;
};

struct QWorld : public QObject
{
    Q_OBJECT
public:
    QWorld(WorldInfo wi, int dim = 0, LayerOpt lopt = LayerOpt());
    virtual ~QWorld();

    void setDim(int dim, LayerOpt lopt);

    void cleancache(std::vector<Quad*>& cache, unsigned int maxsize);

    void draw(QPainter& painter, int vw, int vh, qreal focusx, qreal focusz, qreal blocks2pix);

    int getBiome(Pos p);
    QString getBiomeName(Pos p);

    void refreshBiomeColors();

    void startWorkers();
    void waitForIdle();
    bool isBusy();
    void clear();
    void add(Scheduled *q);
    Scheduled *take(Scheduled *q);
    Scheduled *requestQuad();

signals:
    void update();

public:
    WorldInfo wi;
    int dim;
    LayerOpt lopt;
    Generator g;
    SurfaceNoise sn;

    // the visible area is managed in Quads of different scales (for biomes and structures),
    // which are managed in rectangular sections as levels
    std::vector<Level> lvb;     // levels for biomes
    std::vector<Level> lvs;     // levels for structures
    int activelv;

    // processed Quads are cached until they are too far out of view
    std::vector<Quad*> cachedbiomes;
    std::vector<Quad*> cachedstruct;
    uint64_t memlimit;
    QMutex mutex;
    Scheduled *queue;
    std::vector<MapWorker> workers;
    int threadlimit;

    bool sshow[STRUCT_NUM];
    bool showBB;
    int gridspacing;
    int gridmultiplier;

    // some features such as the world spawn and strongholds will be filled by
    // a designated worker thread once results are done
    QAtomicPointer<Pos> spawn;
    QAtomicPointer<PosElement> strongholds;
    QAtomicPointer<QVector<QuadInfo>> qsinfo;
    // isdel is a flag for the worker thread to stop
    std::atomic_bool isdel;

    // slime overlay
    QImage slimeimg;
    long slimex, slimez;

    // structure selection from mouse position
    bool seldo;
    qreal selx, selz;
    int selopt;
    VarPos selvp;

    qreal qual; // quality, i.e. maximum pixels per 'block' at the current layer
};


#endif // WORLD_H
