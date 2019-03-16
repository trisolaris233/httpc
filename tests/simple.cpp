#include "../src/httpc.hpp"
#include <filesystem>
#include <iostream>
int main() {
    httpc::Server main_server(
        "0.0.0.0",
        "2333",
        "/home/sakakiyukiho/www"
    );

    main_server.AddRouter("/", [&main_server](httpc::Request& req, httpc::Response& res){
        res.RenderFromStaticFile(main_server.GetDocumentRoot() + "/css_naked.html");
    });

    main_server.AddRouter("/list", [&main_server](httpc::Request& req, httpc::Response& res){
        std::string resstr("<html><body><ul>");
        auto root(main_server.GetDocumentRoot());
        for (auto& article : 
            std::filesystem::directory_iterator(root + "/articles")) {
            std::string article_path(article.path());
            auto pos = article_path.find_last_of('/');
            std::string filename(article_path.substr(pos + 1));
            resstr
                .append("<li><a href=\"")
                .append(std::string("articles/") + filename)
                .append("\">")
                .append(filename).append("</a></li>");
        }
        resstr.append("</ul></body></html>");
        res.RenderString(resstr);
    });
    
    main_server.Start();
    for(;;){}

    return 0;
}