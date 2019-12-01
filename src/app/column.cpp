#include "column.h"
#include "util.h"
#include <boost/process.hpp>
#include <iostream>

using namespace std;
enum Icon {
  folderClosed,
  folderOpened,
  folderSymlink,
  file,
  fileSymlink,
  fileHidden,
  excel,
  word,
  ppt,
  stylus,
  sass,
  html,
  xml,
  ejs,
  css,
  webpack,
  markdown,
  json,
  javascript,
  javascriptreact,
  ruby,
  php,
  python,
  coffee,
  mustache,
  conf,
  image,
  ico,
  twig,
  c,
  h,
  haskell,
  lua,
  java,
  terminal,
  ml,
  diff,
  sql,
  clojure,
  edn,
  scala,
  go,
  dart,
  firefox,
  vs,
  perl,
  rss,
  fsharp,
  rust,
  dlang,
  erlang,
  elixir,
  mix,
  vim,
  ai,
  psd,
  psb,
  typescript,
  typescriptreact,
  julia,
  puppet,
  vue,
  swift,
  gitconfig,
  bashrc,
  favicon,
  docker,
  gruntfile,
  gulpfile,
  dropbox,
  license,
  procfile,
  jquery,
  angular,
  backbone,
  requirejs,
  materialize,
  mootools,
  vagrant,
  svg,
  font,
  text,
  archive,
};

extern unordered_map<string, Icon> extensions;
extern unordered_map<string, Icon> filenames;
FileItem::FileItem()
{
}
Cell::Cell()
{
}
Cell::Cell(const Config &cfg, const FileItem& fileitem, const int type)
{
    // https://stackoverflow.com/questions/10681929/how-can-i-determine-the-owner-of-a-file-or-directory-using-boost-filesystem
    if (type==MARK) {
        if ((fileitem.fi.permissions()&boost::filesystem::owner_write)==boost::filesystem::owner_write){
            text = " ";
        }
        else {
            text = mark_indicators["readonly_icon"];
            color = BROWN;
        }
    }
    else if (type == INDENT) {
        // NOTE: text="" when level<0.
        // text = QByteArray(fileitem.level*2, ' ');
        int margin = 1;
        string prefix(margin*2, ' ');
        text.clear();
        const FileItem* pf = fileitem.parent;
        // from high level to low
        if (fileitem.level>0) {
            if (fileitem.last)
                text.append("└ ");
            else
                text.append("│ ");
        
            text.insert(0, prefix);
            for (int i = 0; i<fileitem.level-1; ++i, pf=pf->parent){
                if(pf->last)
                    text.insert(0, "  ");
                else
                    text.insert(0, "│ ");
                text.insert(0, prefix);
            }
        }
        // color = BLUE;
    }
    else if (type == GIT) {
        update_git(fileitem);
    }
    else if (type == ICON) {
        update_icon(fileitem);
    }
    else if (type == FILENAME) {
        color = YELLOW;
        string filename(fileitem.filename);
        if (is_directory(fileitem.fi)) {
            filename.append("/");
            color = BLUE;
        }
        text = filename;
    }
    else if (type == SIZE) {
        update_size(fileitem);
    }
    else if (type == TIME) {
        const file_status & fi = fileitem.fi;
        // TODO: custom column time format
        // text = fi.lastModified().toString("dd.MM.yyyy").toUtf8();
        color = BLUE;
    }
    else{
    }
}
Cell::~Cell()
{
}


unordered_map<string, git_status> FileItem::git_map;
void Cell::update_git(const FileItem &fi)
{
    text = " ";
    string path = fi.p.string();
    // qDebug() << "query:" << path;
    auto search = FileItem::git_map.find(path);
    if (search != FileItem::git_map.end()) {
        auto key = search->second;
        text = git_indicators[key][0];
        color = key;
    }
}

// Checked
void Cell::update_size(const FileItem &fi)
{
    // https://stackoverflow.com/questions/45169587/boostfilesystem-recursively-getting-size-of-each-file
    if (is_regular_file(fi.p)) {
        auto sz = boost::filesystem::file_size(fi.p);

        char text[8];
        if (0 <= sz && sz < 1024) {
            sprintf(text, "%4lu  B", sz);
        }
        else if (1024 <= sz && sz < 1024 * 1024) {
            sz >>= 10;
            sprintf(text, "%4lu KB", sz);
        }
        else if (1024 * 1024 <= sz && sz < 1024 * 1024 * 1024) {
            sz >>= 20;
            sprintf(text, "%4lu MB", sz);
        }
        else if (1024 * 1024 * 1024 <= sz && sz < 1024LL * 1024 * 1024 * 1024) {
            sz >>= 30;
            sprintf(text, "%4lu GB", sz);
        }
        else if (1024LL * 1024 * 1024 * 1024 <= sz &&
                sz < 1024LL * 1024 * 1024 * 1024 * 1024) {
            sz >>= 40;
            sprintf(text, "%4lu TB", sz);
        }
        else {
        }

        this->text = text;
    }
    else this->text = string(7, ' ');
}

// Checked
void Cell::update_icon(const FileItem & fn)
{
    const file_status &fi = fn.fi;
    string suffix = boost::filesystem::extension(fn.filename);

    auto search = extensions.find(suffix);

    if (boost::filesystem::is_directory(fi)){
        if (fn.opened_tree) {
            text = "";
            color = folderOpened;
        } else if (boost::filesystem::is_symlink(fi)){
            // directory_symlink_icon ''
            text = "";
            color = folderSymlink;
        } else {
            // directory_icon ''
            // text = icons[folderClosed][0];
            text = "";
            color = folderClosed;
        }
    } else if (search != extensions.end()) {
        text = icons[search->second][0];
        color = search->second;
    } else {
        // default_icon ''
        text = "";
        color = file;
    }
}

/// Ref to https://git-scm.com/docs/git-status
// Checked
git_status get_indicator_name(const char X, const char Y)
{
    if (X == '?' and Y == '?')
        return Untracked;
    else if (X == ' ' and Y == 'M')
        return Modified;
    else if (X == 'M' or X == 'A' or X == 'C')
        return Staged;
    else if (X == 'R') // todo
        return Renamed;
    else if (X == '!')
        return Ignored;
    else if (X == 'U' or Y == 'U' or (X == 'A' and Y == 'A') or
             (X == 'D' and Y == 'D'))
        return Unmerged; // all included
    else if (Y == 'D')
        return Deleted;
    else
        return Unknown;
}

void FileItem::update_gmap(string p)
{
    using namespace boost::process;
    FileItem::git_map.clear();

    ipstream pipe_stream;
    string cmd1 = "git -C " + p + " rev-parse --show-toplevel";
    child c(cmd1, std_out > pipe_stream);

    std::string line;

    if (pipe_stream && std::getline(pipe_stream, line) && !line.empty())
        std::cerr << line << std::endl;
    if (line=="")
        return;
    string topdir(line+boost::filesystem::path::preferred_separator);
    std::cout << __FUNCTION__ << " top dir: " << topdir << std::endl;

    ipstream pipe_stream2;
    string cmd2 = "git -C " + p + " status --porcelain -u";
    std::cerr << cmd2 << std::endl;
    child c2(cmd2, std_out > pipe_stream2);
    vector<string> lines;
    while (pipe_stream2 && std::getline(pipe_stream2, line) && !line.empty()) {
        std::cerr << line << std::endl;
        lines.push_back(line);
    }
    c.wait();
    c2.wait();

    for (int i = 0; i<lines.size();++i) {
        string & line = lines[i];
        if (line.size()<4) continue;
        char X=line[0], Y=line[1];

        git_status status = get_indicator_name(X, Y);
        // TODO: Check compatible in Windows
        if (status == Renamed) {
            std::string::size_type n = line.find(" -> ");
            string key = topdir + line.substr(n+4);
            FileItem::git_map[key] = status;
        }
        else {
            string key = topdir + line.substr(3);
            std::cout << key << std::endl;
            FileItem::git_map[key] = status;
        }
    }
    std::cout << "-----------------------------------" << std::endl;
    /////////////////////////////

    // QStringList lst = res.split("\n");
    // // qDebug().noquote()<<lst;
    // QDir top_dir(topdir);
    // for (int i = 0; i < lst.size(); ++i) {
    //     QString & line = lst[i];
    //     if (line.size()<4) continue;
    //
    //     QChar X=line[0], Y=line[1];
    //     // QStringRef line(&lst[i], 3, line.length() - 3);
    //     QString remain = line.mid(3);
    //     QStringList LR = remain.split(" -> ");
    //     // TODO: how to reverse escape
    //     // qDebug()<<"LR"<<LR;
    //
    //     git_status status = get_indicator_name(X, Y);
    //     // TODO: Check compatible in Windows
    //     QString key = top_dir.filePath(status == Renamed ? LR[1] : LR[0]);
    //     QDir keyDir(key);
    //     while (keyDir!=top_dir) {
    //         FileItem::git_map[keyDir.absolutePath()] = status;
    //         keyDir.cdUp();
    //     }
    // }
    // foreach (const QString &name, git_map.keys())
    //     qDebug().noquote() << name << ":" << git_map.value(name);
    //
    // proc.close();
}

Context::Context(const Map &ctx)
{
    for (auto i : ctx) {
        auto k = i.first.as_string();
        auto v = i.second;
        if (k == "prev_bufnr") {
            prev_bufnr = v.as_uint64_t();
        }else if (k == "cursor") {
            cursor = v.as_uint64_t();
        }else if (k == "prev_winid") {
            prev_winid = v.as_uint64_t();
        }else if (k == "visual_start") {
            visual_start = v.as_uint64_t();
        }else if (k == "visual_end") {
            visual_end = v.as_uint64_t();
        }
        else{
            cout<<"Unsupported member: "<<k <<endl;
        }
    }
}

// https://www.zhihu.com/question/36642771
void ssplit(const string &s, vector<string> &tokens, const string &delimiters=" ")
{
    string::size_type lastPos = s.find_first_not_of(delimiters, 0);
    string::size_type pos = s.find_first_of(delimiters, lastPos);
    while (string::npos != pos || string::npos != lastPos) {
        // use emplace_back after C++11
        tokens.emplace_back(s.substr(lastPos, pos-lastPos));
        lastPos = s.find_first_not_of(delimiters, pos);
        pos = s.find_first_of(delimiters, lastPos);
    }
}
Config::Config(const Map &ctx) {
    update(ctx);
}
void Config::update(const Map &ctx)
{
    for (auto i : ctx) {
        auto k = i.first.as_string();
        auto &v = i.second;
        // std::cout << __FUNCTION__ << k << " type: "<< type_name(v)<< std::endl;
        if (k == "auto_recursive_level") {
            auto_recursive_level = v.as_int64_t();
        }
        else if (k == "wincol") {
            wincol = v.as_uint64_t();
        }
        else if (k == "winheight") {
            winheight = v.as_int64_t();
        }
        else if (k == "winrow") {
            winrow = v.as_uint64_t();
        }
        else if (k == "winwidth") {
            winwidth = v.as_uint64_t();
        }
        else if (k == "auto_cd") {
            auto_cd = v.as_bool();
        }
        else if (k == "listed") {
            listed = v.as_bool();
        }
        else if (k == "new") {
            new_ = v.as_bool();
        }
        else if (k == "profile") {
            profile = v.as_bool();
        }
        else if (k == "show_ignored_files") {
            show_ignored_files = v.as_bool();
        }
        else if (k == "toggle") {
            toggle = v.as_bool();
        }
        else if (k == "root_marker") {
            root_marker = v.as_string();
        }
        else if (k == "buffer_name") {
            buffer_name = v.as_string();
        }
        else if (k == "direction") {
            direction = v.as_string();
        }
        else if (k == "ignored_files") {
            ignored_files = v.as_string();
        }
        else if (k == "search") {
            search = v.as_string();
        }
        else if (k == "session_file") {
            session_file = v.as_string();
        }
        else if (k == "sort") {
            sort = v.as_string();
        }
        else if (k == "winrelative") {
            winrelative = v.as_string();
        }
        else if (k == "split") {
            split = v.as_string();
        }
        else if (k == "columns") {
            vector<string> tokens;
            ssplit(v.as_string(), tokens, ":");
            columns.clear();
            for (const string &col : tokens) {
                if (col == "mark") columns.push_back(MARK);
                else if (col=="indent") columns.push_back(INDENT);
                else if (col=="git") columns.push_back(GIT);
                else if (col=="icon") columns.push_back(ICON);
                else if (col=="filename") columns.push_back(FILENAME);
                else if (col=="size") columns.push_back(SIZE);
                else if (col=="time") columns.push_back(TIME);
            }
        }
        else{
            cout<<"Unsupported member: "<<k<<endl;
        }
    }
}

unordered_map<string, string> mark_indicators = {
    {"readonly_icon", "✗"},
    {"selected_icon", "✓"},
};

array<string, 2> git_indicators[] =  {
    {{"✭", "#FFFFFF"}}, // Untracked
    {{"✹", "#fabd2f"}}, // Modified
    {{"✚", "#b8bb26"}}, // Staged
    {{"➜", "#fabd2f"}}, // Renamed
    {{"☒", "#FFFFFF"}}, // Ignored
    {{"═", "#fb4934"}}, // Unmerged
    {{"✖", "#fb4934"}}, // Deleted
    {{"?", "#FFFFFF"}}, // Unknown
};
// clang-format off
string gui_colors[] =  {
    "#905532",
    "#3AFFDB",
    "#689FB6",
    "#44788E",
    "#834F79",
    "#834F79",
    "#AE403F",
    "#F5C06F",
    "#F09F17",
    "#D4843E",
    "#F16529",
    "#CB6F6F",
    "#EE6E73",
    "#8FAA54",
    "#31B53E",
    "#FFFFFF"
};

array<string, 2> icons[] = {
    { {"", "#00afaf"} },
    { {"", "#00afaf"} },
    { {"", "#00afaf"} },
    { {"", "#999999"} },
    { {"", "#999999"} },
    { {"﬒", "#999999"} },
    { {"", "#207245"} },
    { {"", "#185abd"} },
    { {"", "#cb4a32"} },
    { {"", "#8dc149"} },
    { {"", "#f55385"} },
    { {"", "#e37933"} },
    { {"謹", "#e37933"} },
    { {"", "#cbcb41"} },
    { {"", "#519aba"} },
    { {"ﰩ", "#519aba"} },
    { {"", "#519aba"} },
    { {"", "#cbcb41"} },
    { {"", "#cbcb41"} },
    { {"", "#519aba"} },
    { {"", "#cc3e44"} },
    { {"", "#a074c4"} },
    { {"", "#519aba"} },
    { {"", "#cbcb41"} },
    { {"", "#e37933"} },
    { {"", "#6d8086"} },
    { {"", "#a074c4"} },
    { {"", "#cbcb41"} },
    { {"", "#8dc149"} },
    { {"", "#519aba"} },
    { {"", "#a074c4"} },
    { {"", "#a074c4"} },
    { {"", "#519aba"} },
    { {"", "#cc3e44"} },
    { {"", "#4d5a5e"} },
    { {"λ", "#e37933"} },
    { {"", "#41535b"} },
    { {"", "#f55385"} },
    { {"", "#8dc149"} },
    { {"", "#519aba"} },
    { {"", "#cc3e44"} },
    { {"", "#519aba"} },
    { {"", "#03589C"} },
    { {"", "#e37933"} },
    { {"", "#854CC7"} },
    { {"", "#519aba"} },
    { {"", "FB9D3B"} },
    { {"", "#519aba"} },
    { {"", "#519aba"} },
    { {"", "#cc3e44"} },
    { {"", "#A90533"} },
    { {"", "#a074c4"} },
    { {"", "#cc3e44"} },
    { {"", "#019833"} },
    { {"", "#cbcb41"} },
    { {"", "#519aba"} },
    { {"", "#519aba"} },
    { {"", "#519aba"} },
    { {"", "#519aba"} },
    { {"", "#a074c4"} },
    { {"", "#cbcb41"} },
    { {"﵂", "#8dc149"} },
    { {"", "#e37933"} },
    { {"", "#41535b"} },
    { {"", "#4d5a5e"} },
    { {"", "#cbcb41"} },
    { {"", "#519aba"} },
    { {"", "#e37933"} },
    { {"", "#cc3e44"} },
    { {"", "#0061FE"} },
    { {"", "#cbcb41"} },
    { {"", "#a074c4"} },
    { {"", "#1B75BB"} },
    { {"", "#E23237"} },
    { {"", "#0071B5"} },
    { {"", "#F44A41"} },
    { {"", "#EE6E73"} },
    { {"", "#ECECEC"} },
    { {"", "#1563FF"} },
    { {"ﰟ", "#FFB13B"} },
    { {"", "#999999"} },
    { {"", "#999999"} },
    { {"", "#cc3e44"} },
};

unordered_map<string, Icon> extensions = {
    { "styl", stylus },
    { "sass", sass },
    { "scss", sass },
    { "htm", html },
    { "html", html },
    { "slim", html },
    { "xml", xml },
    { "xaml", xml },
    { "ejs", ejs },
    { "css", css },
    { "less", css },
    { "md", markdown },
    { "mdx", markdown },
    { "markdown", markdown },
    { "rmd", markdown },
    { "json", json },
    { "js", javascript },
    { "es6", javascript },
    { "jsx", javascriptreact },
    { "rb", ruby },
    { "ru", ruby },
    { "php", php },
    { "py", python },
    { "pyc", python },
    { "pyo", python },
    { "pyd", python },
    { "coffee", coffee },
    { "mustache", mustache },
    { "hbs", mustache },
    { "config", conf },
    { "conf", conf },
    { "ini", conf },
    { "yml", conf },
    { "yaml", conf },
    { "toml", conf },
    { "jpg", image },
    { "jpeg", image },
    { "bmp", image },
    { "png", image },
    { "gif", image },
    { "ico", ico },
    { "twig", twig },
    { "cpp", c },
    { "c++", c },
    { "cxx", c },
    { "cc", c },
    { "cp", c },
    { "c", c },
    { "h", h },
    { "hpp", h },
    { "hxx", h },
    { "hs", haskell },
    { "lhs", haskell },
    { "lua", lua },
    { "java", java },
    { "jar", java },
    { "sh", terminal },
    { "fish", terminal },
    { "bash", terminal },
    { "zsh", terminal },
    { "ksh", terminal },
    { "csh", terminal },
    { "awk", terminal },
    { "ps1", terminal },
    { "bat", terminal },
    { "cmd", terminal },
    { "ml", ml },
    { "mli", ml },
    { "diff", diff },
    { "db", sql },
    { "sql", sql },
    { "dump", sql },
    { "accdb", sql },
    { "clj", clojure },
    { "cljc", clojure },
    { "cljs", clojure },
    { "edn", edn },
    { "scala", scala },
    { "go", go },
    { "dart", dart },
    { "xul", firefox },
    { "sln", vs },
    { "suo", vs },
    { "pl", perl },
    { "pm", perl },
    { "t", perl },
    { "rss", rss },
    { "f#", fsharp },
    { "fsscript", fsharp },
    { "fsx", fsharp },
    { "fs", fsharp },
    { "fsi", fsharp },
    { "rs", rust },
    { "rlib", rust },
    { "d", dlang },
    { "erl", erlang },
    { "hrl", erlang },
    { "ex", elixir },
    { "exs", elixir },
    { "exx", elixir },
    { "leex", elixir },
    { "vim", vim },
    { "ai", ai },
    { "psd", psd },
    { "psb", psd },
    { "ts", typescript },
    { "tsx", javascriptreact },
    { "jl", julia },
    { "pp", puppet },
    { "vue", vue },
    { "swift", swift },
    { "xcplayground", swift },
    { "svg", svg },
    { "otf", font },
    { "ttf", font },
    { "fnt", font },
    { "txt", text },
    { "text", text },
    { "zip", archive },
    { "tar", archive },
    { "gz", archive },
    { "gzip", archive },
    { "rar", archive },
    { "7z", archive },
    { "iso", archive },
    { "doc", word },
    { "docx", word },
    { "docm", word },
    { "csv", excel },
    { "xls", excel },
    { "xlsx", excel },
    { "xlsm", excel },
    { "ppt", ppt },
    { "pptx", ppt },
    { "pptm", ppt },
};

unordered_map<string, Icon> filenames = {
    { "gruntfile", gruntfile },
    { "gulpfile", gulpfile },
    { "gemfile", ruby },
    { "guardfile", ruby },
    { "capfile", ruby },
    { "rakefile", ruby },
    { "mix", mix },
    { "dropbox", dropbox },
    { "vimrc", vim },
    { ".vimrc", vim },
    { ".gvimrc", vim },
    { "_vimrc", vim },
    { "_gvimrc", vim },
    { "license", license },
    { "procfile", procfile },
    { "Vagrantfile", vagrant },
    { "docker-compose.yml", docker },
    { ".gitconfig", gitconfig },
    { ".gitignore", gitconfig },
    { "webpack", webpack },
    { ".bashrc", bashrc },
    { ".zshrc", bashrc },
    { ".bashprofile", bashrc },
    { "favicon.ico", favicon },
    { "dockerfile", docker },
};

unordered_map<string, Icon> patternMatches = {
    { ".*jquery.*.js$", jquery },
    { ".*angular.*.js$", angular },
    { ".*backbone.*.js$", backbone },
    { ".*require.*.js$", requirejs },
    { ".*materialize.*.js$", materialize },
    { ".*materialize.*.css$", materialize },
    { ".*mootools.*.js$", mootools },
};

// clang-format on
