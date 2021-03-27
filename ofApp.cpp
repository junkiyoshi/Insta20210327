#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openFrameworks");

	ofBackground(239);
	ofSetLineWidth(3);
	ofEnableDepthTest();

	this->cap_size = cv::Size(960, 540);

	this->image.allocate(this->cap_size.width, this->cap_size.height, OF_IMAGE_COLOR);
	this->frame = cv::Mat(cv::Size(this->image.getWidth(), this->image.getHeight()), CV_MAKETYPE(CV_8UC3, this->image.getPixels().getNumChannels()), this->image.getPixels().getData(), 0);

	this->cap.open("D:\\MP4\\Pexels Videos 2880.mp4");
	this->number_of_frames = this->cap.get(cv::CAP_PROP_FRAME_COUNT);
	for (int i = 0; i < this->number_of_frames; i++) {

		cv::Mat src, tmp;
		this->cap >> src;
		if (src.empty()) {

			continue;
		}

		cv::resize(src, tmp, this->cap_size);
		cv::cvtColor(tmp, tmp, cv::COLOR_BGR2RGB);

		this->frame_list.push_back(tmp);
	}
}

//--------------------------------------------------------------
void ofApp::update() {

	int frame_index = (int)(ofGetFrameNum() * 0.42) % this->number_of_frames;
	this->frame_list[frame_index].copyTo(this->frame);
	this->image.update();

	this->mesh_list.clear();
	this->line_list.clear();

	float radius = 250;
	int deg_span = 3;
	int x_span = 80;
	for (int x = 0; x < this->cap_size.width; x += x_span) {

		ofMesh mesh, line;
		line.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
		auto noise_value = ofNoise(x * 0.003 + ofGetFrameNum() * 0.03);
		int deg_start = 0;
		if (noise_value < 0.35) { deg_start = ofMap(noise_value, 0, 0.35, -360, 0); }
		else if (noise_value > 0.65) { deg_start = ofMap(noise_value, 0.65, 1, 0, 360); }

		for (int deg = deg_start; deg < deg_start + 180; deg += deg_span) {

			int index = mesh.getNumVertices();

			vector<glm::vec3> vertices;
			vertices.push_back(glm::vec3(x - this->cap_size.width * 0.5, radius * cos(deg * DEG_TO_RAD), radius * sin(deg * DEG_TO_RAD)));
			vertices.push_back(glm::vec3(x - this->cap_size.width * 0.5, radius * cos((deg + deg_span) * DEG_TO_RAD), radius * sin((deg + deg_span) * DEG_TO_RAD)));
			vertices.push_back(glm::vec3(x + x_span - this->cap_size.width * 0.5, radius * cos((deg + deg_span) * DEG_TO_RAD), radius * sin((deg + deg_span) * DEG_TO_RAD)));
			vertices.push_back(glm::vec3(x + x_span - this->cap_size.width * 0.5, radius * cos(deg * DEG_TO_RAD), radius * sin(deg * DEG_TO_RAD)));

			mesh.addVertices(vertices);

			int y = ofMap(deg - deg_start, 0, 180, 0, this->cap_size.height);
			int next_y = ofMap(deg + deg_span - deg_start, 0, 180, 0, this->cap_size.height);

			mesh.addTexCoord(glm::vec3(x, y, 0));
			mesh.addTexCoord(glm::vec3(x, next_y, 0));
			mesh.addTexCoord(glm::vec3(x + x_span, next_y, 0));
			mesh.addTexCoord(glm::vec3(x + x_span, y, 0));

			mesh.addIndex(index + 0); mesh.addIndex(index + 1); mesh.addIndex(index + 2);
			mesh.addIndex(index + 0); mesh.addIndex(index + 2); mesh.addIndex(index + 3);

			if (deg_start != 0) {

				line.addVertices(vertices);
				line.addIndex(index + 0); line.addIndex(index + 1);
				line.addIndex(index + 2); line.addIndex(index + 3);
			}
		}

		if (line.getNumVertices() > 0) {

			line.addIndex(0); line.addIndex(2);
			line.addIndex(line.getNumVertices() - 1); line.addIndex(line.getNumVertices() - 3);
		}

		this->mesh_list.push_back(mesh);
		this->line_list.push_back(line);
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->cam.begin();

	for (int i = 0; i < this->mesh_list.size(); i++) {

		float z = (this->mesh_list[i].getVertex(0).z + this->mesh_list[i].getVertex(2).z) / 2;

		this->image.bind();
		ofSetColor(255);
		this->mesh_list[i].draw();
		this->image.unbind();

		ofSetColor(39);
		this->line_list[i].drawWireframe();
	}

	this->cam.end();
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}