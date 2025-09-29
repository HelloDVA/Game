#include "../utils/buffer.h"

#include <algorithm>
#include <string_view>

#include "httpparser.h"
#include "httprequest.h"

bool HttpParser::Parse(Buffer& input_buffer, HttpRequest& request) {
  	while (state_ != kParseDone && state_ != kParseError) {

		// 数据不足，没找到请求行结束符
		if (state_ == kParseRequestLine) {
			const char* crlf = input_buffer.FindCRLF();
			if (!crlf) {
				return false; 
			}

			// 解析 Request Line
			const char* read_start = input_buffer.Peek();
			bool success = ParseRequestLine(read_start, crlf, request);
			if (!success) {
				state_ = kParseError;
				return false;
			}

			// 消耗掉已解析的数据,更改状态
			input_buffer.Retrieve(crlf + 2 - read_start); 			
			state_ = kParseHeaders;

		} else if (state_ == kParseHeaders) {

			// 数据不足，没找到下一行的结束符
			const char* crlf = input_buffer.FindCRLF();
			if (!crlf) {
				return false; 			
			}
			
			// While loop 一行一行地处理请求头
			const char* read_start = input_buffer.Peek();

			// 如果处理到了head的最后一行，判断是否处理好了。
			// 否则调用函数进行处理。
			if (crlf == read_start) { 
				input_buffer.Retrieve(crlf + 2 - read_start);

				// 检查头部，判断是否有Body（例如检查Content-Length）
				// 没有Body，解析完成
				if (request.getheader("Content-Length").empty()) {
					state_ = kParseDone; 
				} else {
					body_length_ = std::stoi(request.getheader("Content-Length"));
					state_ = kParseBody; // 有Body，进入Body解析状态
				}

			} else {
				ParseHeaderLine(read_start, crlf, request);
				input_buffer.Retrieve(crlf + 2 - read_start);
			}

		} else if (state_ == kParseBody) {

			// Body数据还没收全
			if (input_buffer.ReadableBytes() < body_length_) {
				return false; 
			}

			// 读取指定长度的Body数据
			request.setbody(std::string(input_buffer.Peek(), body_length_));
			input_buffer.Retrieve(body_length_);
			state_ = kParseDone;
		}
		
		// check the state
		if (state_ == kParseDone) {
			state_ = kParseRequestLine; // 重置状态机，为解析下一个请求做准备
			return true;
		}
	}

  return false;
}

bool HttpParser::ParseRequestLine(const char* start, const char* end, HttpRequest& request) {
	// 示例请求行: "GET /index.html HTTP/1.1\r\n"
	
	// Find first space.
	const char* space = std::find(start, end, ' ');
	if (space == end)
		return false;
	request.setmethod(std::string(start, space));	
	
	// Find the second space.
	start = space + 1;
	space = std::find(start, end, ' ');
	if (space == end)
		return false;
	request.setpath(std::string(start, space));	
	
	// The last data and confirm HTTP/.
	start = space + 1;
	if (std::string_view(start, 5) != "HTTP/")
		return false;
	request.setversion(std::string(start, end));
	return true;
}

bool HttpParser::ParseHeaderLine(const char* start, const char* end, HttpRequest& request) {
	// 查找冒号分隔符
    const char* colon = std::find(start, end, ':');
    if (colon == end) {
        //LOG_WARN << "Header line missing colon";
        return false; // 无效的头域，没有冒号
    }

    // 解析字段名 (从 begin 到 colon)
    // 字段名应该不包含前置空格，但规范允许吗？我们修剪一下
    // 修剪字段名尾部的空格（虽然规范不允许，但为了健壮性）
    const char* name_start = start;
    const char* name_end = colon;
    while (name_end > name_start && std::isspace(*(name_end - 1))) {
        name_end--;
    }
    if (name_start == name_end) {
        //LOG_WARN << "Empty header name";
        return false; // 空的字段名
    }
    std::string field_name(name_start, name_end);

    // 修剪值的前导空格 (OWS - Optional Whitespace)
    // 解析字段值 (从 colon+1 到 end)
    const char* value_start = colon + 1;
    const char* value_end = end;
    while (value_start < value_end && std::isspace(*value_start)) {
        value_start++;
    }
    // 修剪值的尾部空格
    while (value_end > value_start && std::isspace(*(value_end - 1))) {
        value_end--;
    }
    if (value_start >= value_end) {
        // 值为空是允许的，比如 "X-Header:"
		request.setheader(field_name, "");
        return true;
    }
    std::string field_value(value_start, value_end);
	
    // 特殊处理：某些头部字段可以出现多次（如Set-Cookie），需要累积
    // 但大多数字段应该覆盖之前的值
    std::string existing_value = request.getheader(field_name);
    if (!existing_value.empty()) {
        // 对于某些字段，我们追加值（用逗号分隔）
        if (field_name == "Cookie" || field_name == "Set-Cookie" || 
            field_name == "Warning" || field_name == "Via") {
            field_value = existing_value + ", " + field_value;
        }
        // 对于其他字段，后面的值覆盖前面的（虽然这通常表示客户端错误）
    }
    request.setheader(field_name, field_value);
    return true;
}
