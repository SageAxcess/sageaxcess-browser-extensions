// $(document).ready(function () {
//     var _fields = [];
//
//     if (!Array.prototype.indexOf) {
//         Array.prototype.indexOf = function (obj, start) {
//             for (var i = (start || 0), j = this.length; i < j; i++) {
//                 if (this[i] === obj) { return i; }
//             }
//             return -1;
//         }
//     }
//
//     $("input[type='password']").each(function (key, val) {
//         var inputs = $(val).closest("form").find("input").toArray();
//         for (var i = inputs.indexOf(val); i >= 0; i--) {
//             if ($(inputs[i]).attr("type") == "text" || $(inputs[i]).attr("type") == "email")
//                 _fields.push($(inputs[i]).attr("name"));
//         }
//     });
//
//     if (_fields.length > 0) {
//         helper.addUsernameMask(_fields);
//     }
// });
