
class Buffer;
class HttpRequest;

class HttpParser {
	public:
		enum ParseState {
			kParseRequestLine,
			kParseHeaders,
			kParseBody,
			kParseDone,
			kParseError
		};
		
		HttpParser() : state_(kParseRequestLine) {}
		~HttpParser() {}

		bool Parse(Buffer& input_buffer, HttpRequest& request);

	private:
		bool ParseRequestLine(const char* start, const char* end, HttpRequest& request);
		bool ParseHeaderLine(const char* start, const char* end, HttpRequest& request);
		void ParseBody();

		ParseState state_;
		int body_length_;
};
