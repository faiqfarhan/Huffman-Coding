#include <cstdlib>
#include <cstdint>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <ctime>

class BitWriter
{
public:
    uint32_t countBits;
    char content;
    std::string allContents;
    std::ofstream* outStream;

    BitWriter(std::ofstream* outStream) : allContents(""), countBits(0), content(0), outStream(outStream)
    {
    }

    void writeBit(bool value)
    {
        if (value == true)
        {
            content <<= 1;
            content |= 1;
            allContents += '1';

        }
        else
        {
            content <<= 1;
            allContents += '0';
        }

        countBits++;

        if (countBits == 8)
        {
            outStream->put(content);
            countBits = 0;
            content = 0;
        }
    }

    void writeByte(char input)
    {
        char temp;
        bool oneBit;

        for (uint8_t i = 1; i <= 8; i++)
        {
            temp = input;
            if (i != 8)
            {
                temp >>= (8 - i);
            }
            temp &= 1;

            if (temp == 0)
            {
                oneBit = false;
            }

            if (temp == 1)
            {
                oneBit = true;
            }


            this->writeBit(oneBit);
        }
    }
};

class BitReader
{
public:
    uint32_t byteNumber;
    uint32_t countBits;
    uint32_t readBits;
    uint32_t totalBits;
    bool outOfBounds;
    char temp;
    std::string allContents;
    std::ifstream* inStream;

    BitReader(std::ifstream* inStream) : countBits(0), readBits(0), inStream(inStream), outOfBounds(false), allContents("")
    {

    }

    bool canReadBit()
    {
        if ((readBits + 1) <= totalBits)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool readBit()
    {
        if (readBits == totalBits)
        {
            std::cout << "You can't read any more.\n";
            system("pause");
            outOfBounds = true;
        }

        if (!outOfBounds)
        {
            bool retValue;
            inStream->seekg(byteNumber);

            char content;

            inStream->get(content);

            temp = 0;

            uint32_t rightShift = 7 - countBits;
            content >>= rightShift;
            content &= 1;

            if (content == 0)
            {
                retValue = false;
                allContents += '0';
            }
            else
            {
                retValue = true;
                allContents += '1';
            }

            countBits++;

            readBits++;

            if (countBits == 8)
            {
                byteNumber ++;
                countBits = 0;
            }

            return retValue;
        }
    }

    char readByte()
    {
        char retValue = 0;
        bool oneBit;

        for (uint8_t i = 1; i <= 8; i++)
        {
            oneBit = this->readBit();

            switch (oneBit)
            {
            case true:
                retValue <<= 1;
                retValue |= 1;
                break;

            case false:
                retValue <<= 1;
                break;
            }
        }

        return retValue;
    }
};

class Node
{
public:
    char letter;
    uint32_t frequency;
    Node* left;
    Node* right;

    Node() : letter(NULL), frequency(NULL), left(NULL), right(NULL) {}

    Node(const Node& rhs)
    {
        this->letter = rhs.letter;
        this->frequency = rhs.frequency;
        this->left = NULL;
        this->right = NULL;
        if (rhs.left)
        {
            this->left = new Node();
            *(this->left) = *(rhs.left);
        }
        if (rhs.right)
        {
            this->right = new Node();
            *(this->right) = *(rhs.right);
        }
    }

    Node& operator=(const Node& rhs)
    {

        if(!(this == &rhs))
        {
            letter = rhs.letter;
            frequency = rhs.frequency;
            if(!(left == NULL)) delete left;
            if(!(right == NULL)) delete right;

            if(rhs.left != NULL)
            {
                left = new Node();
                *left = *(rhs.left);
                if(rhs.right != NULL)
                {
                    right = new Node();
                    *right = *(rhs.right);
                }
            }
            else left = right = NULL;
        }
        return *this;
    }

    bool checkLeafNode()
    {
        if (this->right == NULL && this->left == NULL)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

};

Node* readNode(BitReader& reader)
{
    if (!reader.outOfBounds)
    {
        if (reader.readBit() == true)
        {
            Node* newNode = new Node();
            newNode->letter = reader.readByte();
            return newNode;
        }
        else
        {
            Node* leftChild = readNode(reader);
            Node* rightChild = readNode(reader);
            Node* newNode = new Node();
            newNode->letter = '*';
            newNode->left = leftChild;
            newNode->right = rightChild;
            return newNode;
        }
    }
}

void encodeNode(Node* &node, BitWriter& writer)
{
    if (node->checkLeafNode())
    {
        writer.writeBit(1);
        writer.writeByte(node->letter);
    }
    else
    {
        writer.writeBit(0);
        encodeNode(node->left, writer);
        encodeNode(node->right, writer);
    }
}

class Heap
{
public:
    Node**trees;
    uint32_t size;
    uint32_t currSize;
    std::map<char, std::string> codeTable;

    Heap(uint32_t size) : size(size), currSize(0)
    {
        trees = new Node*[size];
        for (uint32_t i = 0; i < size; i ++)
        {
            trees[i] = new Node();
        }
    }

    void getCode(Node* root, char letter, std::string bitString)
    {
        if (root == NULL)
        {
            return;
        }
        else if (root->left == NULL && root->right == NULL && root->letter == letter)
        {
            codeTable[letter] = bitString;
            // std::cout << letter << ": " << bitString << " ";
            // system("pause");
            return;
        }
        else
        {
            getCode(root->left, letter, bitString + "0");
            getCode(root->right, letter, bitString + "1");
        }
    }

    bool isEmpty()
    {
        return (currSize == 0);
    }

    bool isFull()
    {
        return (currSize == size);
    }

    void display()
    {
        std::cout << "Displaying all nodes' data: " << std::endl;
        for (uint32_t i = 0; i < currSize; i ++)
        {
            std::cout << trees[i]->letter << ": "  << trees[i]->frequency << " ";
        }
        std::cout << std::endl;
    }

    void insert(char letter, uint32_t freq)
    {
        if (!isFull())
        {
            uint32_t hole = currSize;

            for ( ; hole >= 1 && trees[hole / 2]->frequency > freq; hole /= 2)
            {
                trees[hole]->letter = trees[hole / 2]->letter;
                trees[hole]->frequency = trees[hole / 2]->frequency;
            }

            trees[hole]->letter = letter;
            trees[hole]->frequency = freq;

            ++currSize;
        }
    }

    void insert(Node* input)
    {
        if (!isFull())
        {
            uint32_t hole = currSize;

            for ( ; hole >= 1 && trees[hole / 2]->frequency > input->frequency; hole /= 2)
            {
                *(trees[hole]) = *(trees[hole / 2]);
            }


            *(trees[hole]) = *(input);

            currSize++;
        }
    }

    Node* pop()
    {
        Node* retNode = NULL;

        if (!isEmpty())
        {
            --currSize;

            retNode = new Node();
            *retNode = *trees[0];

            *trees[0] = *trees[currSize];

            percDown(0);

            return retNode;
        }
    }

    void percDown(int curIndex)
    {
        int children = 0;
        if(children = numChildren(curIndex))
        {
            Node* temp;
            int left = 2*curIndex+1, right = 2*curIndex+2;
            switch(children)
            {
            case 1:
                if(trees[curIndex]->frequency > trees[left]->frequency)
                {
                    temp = trees[curIndex];
                    trees[curIndex] = trees[left];
                    trees[left] = temp;
                    percDown(left);
                }
                break;
            case 2:
                if((trees[curIndex]->frequency > trees[left]->frequency) ||
                        (trees[curIndex]->frequency > trees[right]->frequency))
                {
                    if((trees[left]->frequency < trees[right]->frequency))
                    {
                        temp = trees[curIndex];
                        trees[curIndex] = trees[left];
                        trees[left] = temp;
                        percDown(left);
                    }
                    else
                    {
                        temp = trees[curIndex];
                        trees[curIndex] = trees[right];
                        trees[right] = temp;
                        percDown(right);
                    }
                }
                break;
            default:
                break;
            }
        }
        return;
    }

    int numChildren(int index)
    {
        int children = 0, left = 2*index+1, right = 2*index+2;
        if(left >= currSize) return 0;
        if(trees[left] != NULL) children++;
        if (trees[right] != NULL) children++;
        return children;
    }


};

void compressionRoutine(std::string fileName)
{

    std::ifstream iStream;
    iStream.open(fileName, std::ios::binary | std::ios::in);

    if (!iStream)
    {
        std::cout << "File couldn't be opened for reading. Try again!" << std::endl;
        iStream.close();
        return;
    }

    iStream.seekg(0, std::ios_base::end);
    uint32_t size = iStream.tellg();
    iStream.seekg(0, std::ios_base::beg);

    if (size == 0)
    {
        std::cout << "It is an empty file." << std::endl;
        return;
    }

    char* txtContents = new char[size];

    iStream.read(txtContents, size);


    if (iStream.gcount() == size)
    {
        std::map<char, uint32_t> freqTable;

        for (uint32_t i = 0; i < size; i ++)
        {
            freqTable[txtContents[i]] += 1;
        }

        Heap* priorityQueue = new Heap(size);

        for (std::map<char, uint32_t>::iterator it = freqTable.begin(); it != freqTable.end(); it++)
        {
            priorityQueue->insert(it->first, it->second);
        }

        Node* n = NULL;
        Node* n1 = NULL;
        Node* n2 = NULL;

        if (priorityQueue->currSize == 1)
        {
            std::cout << "Files with only one character can't be compressed." << std::endl;
            iStream.close();
            return;
        }

        while (priorityQueue->currSize != 1)
        {
            n = new Node();
            n1 = priorityQueue->pop();
            n2 = priorityQueue->pop();

            n->frequency = n1->frequency + n2->frequency;
            n->letter = '*';
            n->left = n1;
            n->right = n2;

            priorityQueue->insert(n);
        }

        for (std::map<char, uint32_t>::iterator it = freqTable.begin(); it != freqTable.end(); it++)
        {
            std::string bitString = "";
            priorityQueue->getCode(priorityQueue->trees[0], it->first, bitString);
        }

        std::ofstream oStream;
        oStream.open(fileName + ".mfj", std::ios::binary | std::ios::out);

        if (!oStream)
        {
            std::cout << "File couldn't be opened for writing. Try again!" << std::endl;
            oStream.close();
            return;

        }

        std::string compressedString = "";

        for (uint32_t i = 0; i < size; i++)
        {
            compressedString += priorityQueue->codeTable[txtContents[i]];
        }

        uint32_t encodedDataBits = 0;
        std::string tempString = "";

        for (std::map<char, uint32_t>::iterator it = freqTable.begin(); it != freqTable.end(); it++)
        {
            tempString = priorityQueue->codeTable[it->first];
            encodedDataBits += it->second * tempString.length();
            tempString = "";
        }

        std::cout << "Encoded data bits: " << encodedDataBits << std::endl;

        uint16_t treeBits = (priorityQueue->codeTable.size() * 10) - 1;
        std::cout << "Tree bits: " << treeBits << std::endl;

        int paddedBits = 8 - ((treeBits + encodedDataBits) % 8);
        std::cout << "Padded bits: " << paddedBits << std::endl;

        BitWriter bW(&oStream);

        char temp = 0;
        bool oneBit = 0;

        for (int i = 1; i <= 8; i++)
        {
            temp = (paddedBits >> (8 - i));
            temp &= 1;
            oneBit = (int) temp;
            bW.writeBit(oneBit);
        }

        temp = 0;
        oneBit = 0;

        for (int i = 1; i <= 16; i ++)
        {
            temp = (treeBits >> (16 - i));
            temp &= 1;
            oneBit = (int) temp;
            bW.writeBit(oneBit);
        }

        encodeNode(priorityQueue->trees[0], bW);

        for (uint32_t i = 0; i < compressedString.length(); i ++)
        {
            if (compressedString[i] == '0')
            {
                bW.writeBit(0);
            }
            else if (compressedString[i] == '1')
            {
                bW.writeBit(1);
            }
        }

        for (int i = 1; i <= paddedBits; i ++)
        {
            bW.writeBit(0);
        }

        std::cout << "The number of bits taken by the original file is: " << size << std::endl;
        uint32_t compressedLength = 8 + 16 + treeBits + encodedDataBits;
        std::cout << "The number of bits taken by the compressed file is: " << compressedLength << std::endl;
        oStream.close();
        std::cout << "Compression done." << std::endl;

    }
    else
    {
        std::cout << "There was an error occured while reading the file." << std::endl;
        iStream.close();
    }

}

void decompressionRoutine(std::string fileName)
{

    std::ifstream iStream;

    iStream.open(fileName, std::ios::binary | std::ios::in);

    if (!iStream)
    {
        std::cout << "File couldn't be opened for reading. Try again!" << std::endl;
        iStream.close();
        return;
    }

    iStream.seekg(0, std::ios_base::end);
    uint32_t fileSize = iStream.tellg();
    iStream.seekg(0, std::ios_base::beg);

    uint32_t fileBits = fileSize * 8;

    BitReader bR(&iStream);
    bR.totalBits = 8;
    bR.byteNumber = 0;
    bR.readBits = 0;

    bool oneBit = 0;
    int paddedBits = 0;

    while (bR.canReadBit())
    {
        oneBit = bR.readBit();
        if (oneBit == true)
        {
            paddedBits <<= 1;
            paddedBits |= 1;
        }
        else
        {
            paddedBits <<= 1;
        }
    }

    std::cout << "Padded bits: " << paddedBits << std::endl;

    uint16_t treeBits = 0;
    oneBit = 0;
    bR.totalBits = 16;
    bR.byteNumber = 1;
    bR.readBits = 0;

    while (bR.canReadBit())
    {
        oneBit = bR.readBit();
        if (oneBit == false)
        {
            treeBits <<= 1;
        }
        else
        {
            treeBits <<= 1;
            treeBits |= 1;
        }
    }

    std::cout << "Tree bits: " << treeBits << std::endl;

    uint32_t encodedBits = fileBits - 8 - 16 - treeBits - paddedBits;
    std::cout << "Encoded data bits: " << encodedBits << std::endl;

    bR.totalBits = treeBits;
    bR.outOfBounds = false;
    bR.readBits = 0;

    Node* root = readNode(bR);

    bR.totalBits = encodedBits;
    bR.readBits = 0;

    Node* temp = root;
    // system("pause");

    std::string originalContent;

    while (1)
    {
        if (bR.canReadBit())
        {
            oneBit = bR.readBit();
            if (oneBit == true)
            {
                temp = temp->right;
            }
            else
            {
                temp = temp->left;
            }

            if (temp->checkLeafNode())
            {
                originalContent += temp->letter;
                temp = root;
            }
        }
        else
        {
            break;
        }
    }

    iStream.close();

    std::ofstream oStream;
    fileName = "UNCOMPRESSED - " + fileName.substr(0, fileName.length() - 4);
    oStream.open(fileName, std::ios::binary | std::ios::out);

    if (!oStream)
    {
        std::cout << "File couldn't be opened for writing. Try again!" << std::endl;
        oStream.close();
        return;
    }

    oStream.write(originalContent.c_str(), originalContent.length());
    oStream.close();

    std::cout << "Decompression done." << std::endl;
}

int main()
{

    std::string fileName;
    int choice;

    while (1)
    {

        std::cout << "Type 1 for compression or 2 for decompression or any other key to exit: ";
        std::cin >> choice;

        std::cin.ignore();

        switch (choice)
        {
        case 1:
            std::cout << "Enter your file name (with extension): ";
            std::getline(std::cin, fileName);

            compressionRoutine(fileName);

            break;

        case 2:
            std::cout << "Enter your file name (with extension): ";
            std::getline(std::cin, fileName);

            decompressionRoutine(fileName);
            break;

        default:
            return EXIT_SUCCESS;
        }

        std::cout << std::endl;
    }

    return EXIT_SUCCESS;
}
