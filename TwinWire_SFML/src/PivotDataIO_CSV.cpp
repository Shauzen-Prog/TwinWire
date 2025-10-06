#include "PivotDataIO_CSV.h"
#include "SpriteAnimator.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <system_error> 

// formatear una linea
static std::string toLine(int i, const FrameMeta& fr)
{
    std::ostringstream oss;
    oss << i << ";"
        << fr.rect.position.x << ";" << fr.rect.position.y << ";"
        << fr.rect.size.x     << ";" << fr.rect.size.y     << ";"
        << fr.pivotX          << ";"
        << fr.duration;
    return oss.str();
}

bool PivotDataIO_CSV::save(const SpriteAnimator& anim, const std::string& path)
{
    namespace fs = std::filesystem;
    fs::path p(path);
    
    // 1) Crear la carpeta si hace falta
    if (!p.parent_path().empty()) {
        std::error_code ec;
        fs::create_directories(p.parent_path(), ec);
        if (ec) {
            std::cerr << "[PivotIO] create_directories failed: "
                      << ec.message() << "  dir=" << p.parent_path().string() << "\n";
            return false;
        }
    }

    
    std::ofstream out_file(p, std::ios::trunc);
    out_file.open(path);
    // 2) Abrir archivo (sin re-abrir luego)
    if (!out_file.is_open()) {
        std::cerr << "[PivotIO] cannot open for write: " << fs::absolute(p).string()
                  << "  (cwd=" << fs::current_path().string() << ")\n";
        return false;
    }
    
    // 3) Escribir encabezado + filas
    out_file << "frame;rx;ry;rw;rh;pivot;dur\n";
    const std::vector<FrameMeta>& frames = anim.frames();
    for (int i = 0; i < static_cast<int>(frames.size()); ++i) {
        out_file << toLine(i, frames[i]) << '\n';
    }
    out_file.close();

    std::cerr << "[PivotIO] saved OK: " << fs::absolute(p).string() << "\n";
    return true;
   
}

bool PivotDataIO_CSV::load(SpriteAnimator& anim, const std::string& path, bool overwriteRectAndDur) {
    std::ifstream in_file;
    in_file.open(path);
    if (!in_file.is_open()) return false;

    std::vector<FrameMeta>& frames = anim.framesMutable();

    std::string line;
    // si hay encabezado, lo saltea
    if (std::getline(in_file, line)) {
        // si la primera línea no es encabezado, retrocede
        if (line.rfind("frame;", 0) != 0) {
            // no era encabezado, procesa esta línea tambien
            in_file.clear();
            in_file.seekg(0, std::ios::beg);
        }
    }

    while (std::getline(in_file, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);

        int i; float rx, ry, rw, rh, px, dur;
        char sep;

        // lee: i;rx;ry;rw;rh;px;dur
        if ( (ss >> i >> sep >> rx >> sep >> ry >> sep >> rw >> sep >> rh >> sep >> px >> sep >> dur)
             && i >= 0 && i < static_cast<int>(frames.size()) )
        {
            if (overwriteRectAndDur) {
                frames[i].rect = sf::IntRect{ { static_cast<int>(rx), static_cast<int>(ry) },
                                              { static_cast<int>(rw), static_cast<int>(rh) } };
                frames[i].duration = dur;
            }
            frames[i].pivotX = px;
        }
    }

    in_file.close();
    anim.applyCurrentFrame(); // refleja cambios en el sprite actual
    return true;
}