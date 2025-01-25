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
 * Each node has at most one yes child and at most one no child, corresponding to the responses to the question
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
 * @class AnimalTree
 * @brief This class is responsible for generating the question tree that forms the basis for the game's logic
 * 
 * Making this its own separate class instead of the part of the AnimalGame class enables us to use object lifetimes to reset the memory of the game
 * Under Resource Acquisition Is Initialization, the lifetime of any instance of this class will be controlled by the AnimalGame class
 * When an instance of the AnimalGame class is initialized, it will initialize an instance of this class as well
 */
class AnimalTree {
private:
    std::unique_ptr<Node> root;

public:
    AnimalTree() {
        resetToInitialState();
    }

    /**
     * @brief is utilized with a clean root node to build the initial version of the tree for use in the game
     * This function creates the unique state by re-assigning the root of the tree to a new unique pointer
     * If there was a pre-existing question tree in use by the game, re-assigning the root of the tree will cause the old tree to no longer be owned by the instance of the AnimalGame class, marking it for garbage collection
     * This function will then create the initial tree
     */
    void resetToInitialState() {
        root = std::make_unique<Node>("Is your animal warm or cold blooded?");
        root->yes = std::make_unique<Node>(std::make_unique<DynamicAnimal>("Dog"));
        root->no = std::make_unique<Node>(std::make_unique<DynamicAnimal>("Snake"));
    }
    /**
     * @brief public method to allow access to the private root field
     * The root field, pointing to the root node of the question tree, is private
     * This getter method allows the AnimalGame to access the root node of the question tree, essential to causing the game to operate correctly
     */
    Node* getRoot() const {
        return root.get();
    }
    /**
     * @brief traverses the question tree to collect all animals currently in memory
     * This creates a full list of animals and works with the AnimalGame.listAnimals() function to display them to the user
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
};

/**
 * @class AnimalGame
 * @brief class that controls actual in-game operations
 * The class, upon initialization, initializes an instance of the AnimalTree class, and uses it to run the game
 * This class handles the logic to traverse the tree and attempt to guess the users animal
 * It also handles the logic to learn new animals and install them into the question tree (along with new questions)
 * It also displays the post-game menu, as well as 
 */
class AnimalGame {
private:
    AnimalTree tree;
    /**
     * @brief function to control inner-game logic
     * This class uses the tree instance of the AnimalTree class to run game logic
     * The user traverses the tree based on their answers until a leaf node is reached
     * At this point, the game is ready to guess their animal
     * If the guess is correct, the game is over and the user is returned to the post-game menu
     * If the guess is incorrect, the user is prompted for a question to add to a new node in the tree (making the node the game ended on a non-leaf node for future playthroughs), along with a new animal learned by the game
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
     * @brief function to add new animals to the existing question tree
     * This function is called by the askQuestions function
     * After an unsuccessful guess, the user is prompted by this function for the name of their animal, as well as a question that would distinguish it at this point in the tree
     * Both of these values are added to a new node on the tree
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
     * @brief function that runs the post-game menu
     * After each round of the game, this menu is displayed
     * Play Again restarts the game with the current question tree
     * Reset memory resets the question tree to the default state (with one question and two total animals) and automatically begins the game again after
     * List All Animals lists all animals currently in the question tree, available to be guessed by the game, then brings this menu up again
     * Quit exits the program
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
                tree.resetToInitialState();
                std::cout << "Game has been reset to initial state.\n";
                break;
            case 3:
                listAnimals();
                promptAfterRound(); 
                break;
            case 4:
                std::exit(0);
            default:
                std::cout << "Invalid choice. Please try again.\n";
                promptAfterRound();
        }
    }

    /**
     * @brief a function to print all animals known by the game
     * This function works with the AnimalTree.collectAnimals() class to collect all animals in the question tree and display them
     * I created this function to make testing my program easier (without this, you have to play the game again and traverse the tree in the same way to ensure a new animal and question were successfully added to it)
     * The requirements in the homework do not require this, but I think keeping this in will make my homework easier to grade for exactly the same reasons adding it made it easier to test
     */
    void listAnimals() const {
        std::vector<std::string> animals;
        tree.collectAnimals(tree.getRoot(), animals);

        std::cout << "Animals currently in memory:\n";
        for (const auto& animal : animals) {
            std::cout << "- " << animal << "\n";
        }
    }

public:
    /**
     * @brief a function that encapsulates other helper functions of the AnimalGame class to yield the desired core gameplay loop
     */
    void play() {
        std::cout << "Welcome to The Animal Game!\n";

        while (true) {
            askQuestions(tree.getRoot());
            promptAfterRound();
        }
    }
};

int main() {
    AnimalGame game;
    game.play();
    return 0;
}