#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <nlohmann/json.hpp>


using json = nlohmann::json;
enum class BinaryOperator { ADD, SUBTRACT, MULTIPLY, DIVIDE };
enum class UnaryOperator { MIN, SUM, MAX};



class Node {
public:
	virtual ~Node() {}
	virtual std::vector<float> Compute() = 0;
	virtual json Serialize() = 0 ;
};


class BinaryOperatorNode : public Node {
public:
	BinaryOperatorNode(Node* left, Node* right, BinaryOperator op)
		: left_(left), right_(right), op_(op) {}

	virtual std::vector<float> Compute()
	{
		auto left_values = left_->Compute();
		auto right_values = right_->Compute();
		std::vector<float> result;
		result.reserve(left_values.size());
		for (int i = 0; i < left_values.size(); ++i) {
			switch (op_) 
			{
			case BinaryOperator::ADD:
				result.push_back(left_values[i] + right_values[i]);
				break;
			case BinaryOperator::SUBTRACT:
				result.push_back(left_values[i] - right_values[i]);
				break;
			case BinaryOperator::MULTIPLY:
				result.push_back(left_values[i] * right_values[i]);
				break;
			case BinaryOperator::DIVIDE:
				result.push_back(left_values[i] / right_values[i]);
				break;
			}
		}
		return result;
	}
	json Serialize() {
		json data;
		data["Binary_op"] = op_;
		data["left"] = left_->Serialize();
		data["right"] = right_->Serialize();
		return data;
	}
private:
	Node* left_;
	Node* right_;
	BinaryOperator op_;
};

class UnaryOperatorNode : public Node {
public:
	UnaryOperatorNode(Node* input, UnaryOperator op) : input_(input), op_(op) {}
	virtual std::vector<float> Compute() 
	{
		auto values = input_->Compute();
		float ret_value = (int)values[0];
		for (const auto& value : values) 
		{
			switch (op_)
			{
			case UnaryOperator::MAX:
				ret_value = (int)*max_element(values.begin(), values.end());
				break;
			case UnaryOperator::MIN:
				ret_value = (int)*max_element(values.begin(), values.end());
				break;
			case UnaryOperator::SUM:
				ret_value = accumulate(values.begin(), values.end(), 0);
				break;
			}
		}
		if (ret_value < 0) {
			return { 0.0f };
		}
		return { ret_value };
	}

	json Serialize() {
		json data;
		data["input"] = input_->Serialize();
		data["unary_op"] = op_;
		return data;
	}

private:
	Node* input_;
	UnaryOperator op_;
};

class UserDataNode : public Node {
public:
	UserDataNode(const std::vector<float>& values) : values_(values) {}
	UserDataNode(){}

	virtual std::vector<float> Compute() {
		return values_;
	}
	json Serialize() {
		json data;
		data["user_data"] = values_;
		return data;
	}

private:
	std::vector<float> values_;
};

class GeneratorNode : public Node {
public:
	GeneratorNode(int size) : size_(size) {}
	GeneratorNode(std::vector<float>& Randvalues) : Randvalues_(Randvalues) {}
	virtual std::vector<float> Compute() {
		if (Randvalues_.empty()) {
			std::mt19937 generator(0x5702135);
			std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
			Randvalues_.reserve(size_);
			for (int i = 0; i < size_; ++i) {
				Randvalues_.push_back(distribution(generator));
			}
		}
		return Randvalues_;
	}

	json Serialize() {
		json data;
		data["generator_data"] = Randvalues_;
		return data;
	}
private:
	int size_;
	std::vector<float> Randvalues_;
};

void Deserializeroot(const nlohmann::json& j) {
	UnaryOperator op_U = j["unary_op"].get<UnaryOperator>();
	BinaryOperator op_B = j["input"]["Binary_op"].get<BinaryOperator>();
	std::vector<float> user_dataVec = j["input"]["left"]["user_data"].get<std::vector<float>>();
	std::vector<float> Gen_dataVec = j["input"]["right"]["generator_data"].get<std::vector<float>>();

	UserDataNode user_data(user_dataVec);
	GeneratorNode generator(Gen_dataVec);
	BinaryOperatorNode binary_op(&user_data, &generator, op_B);
	UnaryOperatorNode unary_op(&binary_op, op_U);
	auto result = unary_op.Compute();
	std::cout << result[0] << std::endl;
}



int main(int argc, char* argv[]) {

	if (argc == 2) {
		std::string input_file(argv[1]);
		std::ifstream file(input_file);
		if (!file.is_open()) {
			std::cerr << "Failed to open input file: " << input_file << std::endl;
			return 1;
		}

		json input_data;
		file >> input_data;
		std::vector<nlohmann::json> resultjson;
		for (const auto& data : input_data) {
			std::vector<float> user_dataVec = data;
			UserDataNode user_data(user_dataVec);
			GeneratorNode generator(user_dataVec.size());
			BinaryOperatorNode binary_op(&user_data, &generator, BinaryOperator::ADD);
			UnaryOperatorNode unary_op(&binary_op, UnaryOperator::MAX);
			auto result = unary_op.Compute();
			resultjson.push_back(unary_op.Serialize());
			std::cout << result[0] << std::endl;
		}
		nlohmann::json root;
		root["objects"] = resultjson;
		std::ofstream out_file("data.json");
		out_file << root.dump(4);
		out_file.close();

	}
	else if (argc == 3) {
		std::string input_file(argv[1]);
		std::string config_file(argv[2]);
		std::ifstream Infile(input_file);
		std::ifstream Config(config_file);
		if (!Infile.is_open()) {
			std::cerr << "Failed to open input file: " << input_file << std::endl;
			return 1;
		}
		if (!Config.is_open()) {
			std::cerr << "Failed to open config_file : " << input_file << std::endl;
			return 1;
		}

		json input_data;
		json config_data;
		Infile >> input_data;
		Config >> config_data;

		BinaryOperator op_B = config_data["Binary_op"].get<BinaryOperator>();
		UnaryOperator op_U = config_data["Unary_op"].get<UnaryOperator>();

		std::vector<nlohmann::json> resultjson;
		for (const auto& data : input_data) {
			std::vector<float> user_dataVec = data;
			UserDataNode user_data(user_dataVec);
			GeneratorNode generator(user_dataVec.size());
			BinaryOperatorNode binary_op(&user_data, &generator, op_B);
			UnaryOperatorNode unary_op(&binary_op, op_U);
			auto result = unary_op.Compute();
			resultjson.push_back(unary_op.Serialize());
			std::cout << result[0] << std::endl;
		}
		nlohmann::json root;
		root["objects"] = resultjson;
		std::ofstream out_file("data.json");
		out_file << root.dump(4);
		out_file.close();

		// Deserialization
		std::cout << "Deserializing the Nodes\n";
		std::ifstream deserializationFile("data.json");
		json deserializationData;
		deserializationFile >> deserializationData;
		for (const auto& data : deserializationData["objects"]) {
			Deserializeroot(data);
		}


	}
	else {
		std::cerr << "Usage: " << argv[0] << " <input_file> or <Config_file> missing" << std::endl;
		return 1;
	}


	return 0;
}
