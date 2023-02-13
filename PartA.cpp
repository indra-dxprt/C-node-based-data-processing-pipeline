#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <nlohmann/json.hpp>



class Node {
public:
	virtual ~Node() {}
	virtual std::vector<float> Compute() = 0;
};

class BinaryOperatorNode : public Node {
public:
	BinaryOperatorNode(Node* left, Node* right)
		: left_(left), right_(right) {}

	virtual std::vector<float> Compute() {
		auto left_values = left_->Compute();
		auto right_values = right_->Compute();
		std::vector<float> result;
		result.reserve(left_values.size());
		for (int i = 0; i < left_values.size(); ++i) {
			result.push_back(left_values[i] + right_values[i]);
		}
		return result;
	}

private:
	Node* left_;
	Node* right_;
};

class UnaryOperatorNode : public Node {
public:
	UnaryOperatorNode(Node* input) : input_(input) {}

	virtual std::vector<float> Compute() {
		auto values = input_->Compute();
		float max_value = (int)values[0];
		for (const auto& value : values) {
			if (value > max_value) {
				max_value = (int)value;
			}
		}
		if (max_value < 0) {
			return { 0.0f };
		}
		return { max_value };
	}

private:
	Node* input_;
};

class UserDataNode : public Node {
public:
	UserDataNode(const std::vector<float>& values) : values_(values) {}

	virtual std::vector<float> Compute() {
		return values_;
	}

private:
	std::vector<float> values_;
};

class GeneratorNode : public Node {
public:
	GeneratorNode(int size) : size_(size) {}

	virtual std::vector<float> Compute() {
		std::mt19937 generator(0x5702135);
		std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
		std::vector<float> values;
		values.reserve(size_);
		for (int i = 0; i < size_; ++i) {
			values.push_back(distribution(generator));
		}
		return values;
	}

private:
	int size_;
};



using json = nlohmann::json;

int main(int argc, char* argv[]) {

	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
		return 1;
	}

	std::string input_file(argv[1]);
	std::ifstream file(input_file);
	if (!file.is_open()) {
		std::cerr << "Failed to open input file: " << input_file << std::endl;
		return 1;
	}

	json input_data;
	file >> input_data;
	int count = 1;
	for (const auto& data : input_data) {
		std::vector<float> user_dataVec = data;
		UserDataNode user_data(user_dataVec);
		GeneratorNode generator(user_dataVec.size());
		BinaryOperatorNode binary_op(&user_data, &generator);
		UnaryOperatorNode unary_op(&binary_op);
		auto result = unary_op.Compute();
		std::cout << "interation : " << count << "   " << result[0] << std::endl;
		count++;
	}

	return 0;
}
