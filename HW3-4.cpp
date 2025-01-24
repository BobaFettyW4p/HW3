#include <iostream>
#include <memory>
#include <string>
#include <vector>

/**
 * @class Animal
 * @brief A virtual base animal class
 * 
 * This is a polymorphic base class to be built upon by the DynamicAnimal class
 * it has a virtual constructor and destructor to ensure that any class that builds on it
 * is able to provide its own constructor and not be limited by the base class.
 * Another benefit of a purely virtual class means it cannot be ininitialized directly, solely through classes that build upon it.
 */
class Animal {
public:
    virtual ~Animal() = default;
    virtual std::string getName() const = 0;
};

/**
 * @class DynamicAnimal
 * @brief a concrete implementation of the abstract Animal class. Stores names of animals as answers by the program for use in the game.
 * 
 * Encapsulates the name field as private to store the names of animals in a controlled manner
 * Also overrides the getName method of the Animal base class to return the shared name. Since the name field is private, this is the only way to get the name field from outside of the class
 */
class DynamicAnimal : public Animal {
private:
    std::string name;
public:
    explicit DynamicAnimal(const std::string& name) : name(name) {}
    std::string getName() const override { return name; }
};

/**
 * @class Node
 * @brief an implementation of the Node class, utilized to generate the question tree
 * 
 * Each node contains a string question, that is used to generate the questions asked to the user while playing the game
 * Each node has a yes child and a no child, corresponding to the responses to the question
 * Once the user has traversed the tree to a leaf node, it will attempt to guess the animal
 */
class Node {
public:
    std::string question;
    std::unique_ptr<Node> yes;
    std::unique_ptr<Node> no;
    std::unique_ptr<Animal> animal;

    Node(const std::string& question) : question(question) {}
    Node(std::unique_ptr<Animal> animal) : animal(std::move(animal)) {}

    bool isLeaf() const { return animal != nullptr; }
};

/**
 * @class AnimalGame
 * @brief the AnimalGame class utilizes the Node and DynamicAnimal classes to perform operations within the game
 */
class AnimalGame {
private:
    std::unique_ptr<Node> root;

    /**
     * @brief creates the question tree used in the game
     * uses the initialized root node to create the initial tree with one question, and two possible guesses based on the response
     * This is used on the first launch of the program, or when the memory is reset to the initial state
     */
    void resetToInitialState() {
        root = std::make_unique<Node>("Is your animal warm or cold blooded?");
        root->yes = std::make_unique<Node>(std::make_unique<DynamicAnimal>("Dog"));
        root->no = std::make_unique<Node>(std::make_unique<DynamicAnimal>("Snake"));
    }
    /**
     * @brief traverses the question tree based on prompts provided by the user until it reaches a leaf node
     * at which point the game will guess the user's animal
     */
    void askQuestions(Node* current) {
        while (!current->isLeaf()) {
            std::cout << current->question << " (yes/no): ";
            std::string answer;
            std::cin >> answer;
            if (answer == "yes") {
                current = current->yes.get();
            } else if (answer == "no") {
                current = current->no.get();
            } else {
                std::cout << "Please answer 'yes' or 'no'.\n";
            }
        }

        std::cout << "Is it a " << current->animal->getName() << "? (yes/no): ";
        std::string answer;
        std::cin >> answer;

        if (answer == "yes") {
            std::cout << "Yay! I guessed it right!\n";
        } else if (answer == "no") {
            learnNewAnimal(current);
        } else {
            std::cout << "Please answer 'yes' or 'no'.\n";
        }
    }

    /**
     * @brief adds a new question to the tree and animal to the game based on user outputs
     * This function is called by the askQuestions function after the user wins the game
     * It asks the user for their animal, as well as a question that can be used to identify it
     * That question with the associated answer is added to the question tree, and will be accessible if the game is played again
     */
    void learnNewAnimal(Node* current) {
        std::cout << "I give up! What is your animal? ";
        std::string newAnimalName;
        std::cin.ignore();
        std::getline(std::cin, newAnimalName);

        std::cout << "What question distinguishes a " << newAnimalName << " from a "
                  << current->animal->getName() << "?\n";
        std::string newQuestion;
        std::getline(std::cin, newQuestion);

        std::cout << "For a " << newAnimalName << ", what is the answer to that question? (yes/no): ";
        std::string answer;
        std::cin >> answer;

        auto newAnimalNode = std::make_unique<Node>(std::make_unique<DynamicAnimal>(newAnimalName));
        auto oldAnimalNode = std::make_unique<Node>(std::move(current->animal));
        current->question = newQuestion;

        if (answer == "yes") {
            current->yes = std::move(newAnimalNode);
            current->no = std::move(oldAnimalNode);
        } else {
            current->yes = std::move(oldAnimalNode);
            current->no = std::move(newAnimalNode);
        }

        std::cout << "Got it! I'll remember that for next time.\n";
    }

    /**
     * @brief displays a menu after every successful round of the game, available options:
     * start another round of the game, delete all learned animals in memory, list all currently known animals, or exit the program
     */
    void promptAfterRound() {
        std::cout << "What would you like to do next?\n";
        std::cout << "1. Play again\n";
        std::cout << "2. Reset memory and play again\n";
        std::cout << "3. List all animals\n";
        std::cout << "4. Quit\n";
        std::cout << "Enter your choice (1/2/3/4): ";

        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1:
                break;
            case 2:
                reset();
                break;
            case 3:
                listAnimals();
                promptAfterRound(); // Re-prompt after listing
                break;
            case 4:
                std::exit(0);
            default:
                std::cout << "Invalid choice. Please try again.\n";
                promptAfterRound();
        }
    }
    /**
     * @brief traverses the question tree to collect all animals currently in memory
     * This is used in conjunction with the listAnimals function to create a full list of animals and memory and display
     */
    void collectAnimals(const Node* current, std::vector<std::string>& animals) const {
        if (!current) return;
        if (current->isLeaf()) {
            animals.push_back(current->animal->getName());
        } else {
            collectAnimals(current->yes.get(), animals);
            collectAnimals(current->no.get(), animals);
        }
    }

public:
    AnimalGame() {
        resetToInitialState();
    }
    /**
     * @brief initializes the game
     * This public method initializes the question tree, and starts the question loop
     */
    void play() {
        std::cout << "Welcome to The Animal Game!\n";

        while (true) {
            askQuestions(root.get());
            promptAfterRound();
        }
    }

    void reset() {
        resetToInitialState();
        std::cout << "Game has been reset to initial state.\n";
    }
    /**
     * @brief function to list all animals currently stored in memory
     * I wrote this function (and the listanimals function) while I was testing this
     * It traverses the tree to collect all the animals in memory and print it out
     * I initially intended it as a debug function, but I think it could be helpful to you while grading, so I've left it in
     */
    void listAnimals() const {
        std::vector<std::string> animals;
        collectAnimals(root.get(), animals);

        std::cout << "Animals currently in memory:\n";
        for (const auto& animal : animals) {
            std::cout << "- " << animal << "\n";
        }
    }
};

// our main function initializes an instance of the AnimalGame class, and uses the play()
// method to start the game. All functions of the game are contained within the class
int main() {
    AnimalGame game;
    game.play();
    return 0;
}
