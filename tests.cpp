#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <nlohmann/json.hpp>
#include <cassert>


using json = nlohmann::json;
enum class BinaryOperator { ADD, SUBTRACT, MULTIPLY, DIVIDE };
enum class UnaryOperator { MIN, SUM, MAX };



class Node {
public:
	virtual ~Node() {}
	virtual std::vector<float> Compute() = 0;
	virtual json Serialize() = 0;
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
				ret_value = (int)* max_element(values.begin(), values.end());
				break;
			case UnaryOperator::MIN:
				ret_value = (int)* max_element(values.begin(), values.end());
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
	UserDataNode() {}

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

void test1() {
	std::vector<float> user_dataVec = { 1,2,3 };
	std::vector<float> Gen_dataVec = { 0.8897231221199036, 0.07282309979200363, 0.3726326525211334, 0.5071849226951599 };
	UserDataNode user_data(user_dataVec);
	GeneratorNode generator(Gen_dataVec);
	BinaryOperatorNode binary_op(&user_data, &generator, BinaryOperator::ADD);
	UnaryOperatorNode unary_op(&binary_op, UnaryOperator::SUM);
	auto result = unary_op.Compute();
	assert(result[0]  == 6);
}

void test2() {
	std::vector<float> user_dataVec = { 9.0 };
	std::vector<float> Gen_dataVec = { 0.8897231221199036 };
	UserDataNode user_data(user_dataVec);
	GeneratorNode generator(Gen_dataVec);
	BinaryOperatorNode binary_op(&user_data, &generator, BinaryOperator::ADD);
	UnaryOperatorNode unary_op(&binary_op, UnaryOperator::MAX);
	auto result = unary_op.Compute();
	assert(result[0] == 9);
}

void test3() {
	std::vector<float> user_dataVec = { 9.0 };
	std::vector<float> Gen_dataVec = { 0.8897231221199036 };
	UserDataNode user_data(user_dataVec);
	GeneratorNode generator(Gen_dataVec);
	BinaryOperatorNode binary_op(&user_data, &generator, BinaryOperator::MULTIPLY);
	UnaryOperatorNode unary_op(&binary_op, UnaryOperator::MAX);
	auto result = unary_op.Compute();
	assert(result[0] == 8);
}

int main() {
	test1();
	test2();
	test3();
	std::cout << "All tests passed." << std::endl;
	return 0;
}
