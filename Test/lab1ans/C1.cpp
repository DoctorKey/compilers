struct Product {
	int name;
} i;
int main() {
	struct Type {
		struct Product id;
		int t;
	}type;
	i.name = 1;
	type.id.name = 2 + i.name * 5;
	type.t = 4;
	return type.id.name * type.t;
}