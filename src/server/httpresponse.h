#include<string>
#include<map>

#define RESOURCE_DIR "../resources"

class HttpResponse{
	public:
		HttpResponse();
		~HttpResponse();

		void MakeGetResponse(std::string path, std::string version, std::string body);
		void MakeLoginResponse(std::string path, std::string version, std::string body);
		void MakeErrorResponse(int status_code, std::string version);
		void MakeMessageResponse(std::string message, std::string version);

		void MakeGameResponse();
		std::string ToString() const;

		void ParseUrl(std::string body); 
		std::string GenerateSessionId();
	private:
		int status_code_;
		std::string status_message_;
		std::string path_;
		std::string version_;
		std::string body_;

		std::string content_type_;
		std::map<std::string, std::string> headers_;

		static const std::map<int, std::string> status_messages_;

		std::map<std::string, std::string> url_data_;
		std::map<std::string, std::string> user_sessions_;
};

