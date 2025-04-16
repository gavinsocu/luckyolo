# -*- coding: utf-8 -*-
from http.server import BaseHTTPRequestHandler, HTTPServer
from urllib.parse import urlparse, parse_qs
import os
import json

HISTORY_FILE = "history.txt"
IMAGES_DIR = "./images"

class SimpleHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        parsed_path = urlparse(self.path)
        path = parsed_path.path
        query = parse_qs(parsed_path.query)

        if path == "/":
            self.send_html()
        elif path == "/history":
            self.send_history()
        elif path.startswith("/images/"):
            self.send_static_file("." + path)  # 添加这个分支来托管 images 里的文件
        elif path == "/images":
            person_id = query.get("personid", [None])[0]
            if person_id:
                self.send_images_by_person(person_id)
            else:
                self.send_error(400, "Missing personid")
        else:
            self.send_error(404, "Not Found")

    def send_html(self):
        try:
            with open("index.html", "r", encoding="utf-8") as f:
                html = f.read()
            self.send_response(200)
            self.send_header("Content-type", "text/html")
            self.end_headers()
            self.wfile.write(html.encode("utf-8"))
        except Exception as e:
            self.send_error(500, f"Error loading HTML: {e}")

    def send_history(self):
        history_data = []
        if os.path.exists(HISTORY_FILE):
            with open(HISTORY_FILE, "r", encoding="utf-8") as f:
                for line in f:
                    try:
                        line = line.strip()
                        if not line:
                            continue
                        time_part = line.split(" personId")[0].replace("时间:", "").strip()
                        person_id = line.split("personId<")[1].split(">")[0]
                        direction = line.split(">")[1].strip()
                        history_data.append({
                            "time": time_part,
                            "personId": person_id,
                            "direction": direction
                        })
                    except:
                        continue
        self.send_response(200)
        self.send_header("Content-type", "application/json")
        self.end_headers()
        self.wfile.write(json.dumps(history_data).encode("utf-8"))

    def send_images_by_person(self, person_id):
        matching_files = []
        if os.path.exists(IMAGES_DIR):
            for file in os.listdir(IMAGES_DIR):
                if file.startswith(f"personId[{person_id}]_") and file.endswith(".jpg"):
                    matching_files.append(file)
                    print(f"找到Found matching file: {file}")
        self.send_response(200)
        self.send_header("Content-type", "application/json")
        self.end_headers()
        self.wfile.write(json.dumps(matching_files).encode("utf-8"))

    def send_static_file(self, filepath):
        if os.path.exists(filepath) and os.path.isfile(filepath):
            try:
                with open(filepath, "rb") as f:
                    content = f.read()
                self.send_response(200)
                if filepath.endswith(".jpg"):
                    self.send_header("Content-type", "image/jpeg")
                elif filepath.endswith(".png"):
                    self.send_header("Content-type", "image/png")
                else:
                    self.send_header("Content-type", "application/octet-stream")
                self.send_header("Content-Length", str(len(content)))
                self.end_headers()
                self.wfile.write(content)
            except Exception as e:
                self.send_error(500, f"Failed to read static file: {e}")
        else:
            self.send_error(404, "Static file not found")

if __name__ == "__main__":
    server = HTTPServer(("0.0.0.0", 8080), SimpleHandler)
    print("Server started on port 8080...")
    server.serve_forever()
